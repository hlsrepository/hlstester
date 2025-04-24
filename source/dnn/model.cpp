# include "model.h"

Model::Model(Config config)
{
    batchSize = 1;
    ActivationFunctionType actFunction;
    for (int i = 0; i < config.layers.size(); i++)
    {
        LayerType layerType = config.layers[i];
        switch (layerType)
        {
        case dense:
        {
            try
            {
                std::map<std::string, std::string> properties = config.layerProperties[i];

                actFunction = Utils::parseActivationFunction(properties["activation"]);

                float dropout = 0.0;
                if (properties.count("dropout"))
                    dropout = std::stof(properties["dropout"]);

                DenseLayer *layer = new DenseLayer(std::stoi(properties["inputs"]),
                                                   std::stoi(properties["outputs"]),
                                                   batchSize,
                                                   actFunction,
                                                   dropout);
                _layers.emplace_back(layer);
                break;
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
        default:
            break;
        }

        switch (actFunction)
        {
        case ActivationFunctionType::relu:
        {
            Relu *relu = new Relu();
            _activationLayers.emplace_back(relu);
            break;
        }

        case ActivationFunctionType::softmax:
        {
            Softmax *softmax = new Softmax();
            _activationLayers.emplace_back(softmax);
            break;
        }

        default:
        {
            break;
        }
        }
    }

    CategoricalCrossEntropy _loss();
    _optimizer.learningRate = std::stoi(config.trainConfig["lr"]);
    trainEpochs = std::stoi(config.trainConfig["epochs"]);
}

void Model::loadWeights(const std::string &path)
{
    weightsPath = path;
    if (Utils::fileExists(weightsPath))
    {
        int cols = 0, rows = 0;
        float buff[MAXBUFSIZE];

        std::ifstream filestream(weightsPath);
        if (filestream.is_open())
        {
            std::string line, token;
            int layer = 0;
            int pos = 0;
            Eigen::MatrixXf *weights;
            while (std::getline(filestream, line))
            {
                if (line.find("layer") != std::string::npos)
                {
                    while (std::getline(filestream, line))
                    {
                        if (line == "")
                        {
                            Eigen::MatrixXf m = Utils::bufferToMatrix(buff, (*(_layers[layer]->_weights)).rows(), (*(_layers[layer]->_weights)).cols());
                            *(_layers[layer]->_weights) = m;
                            pos = 0;
                            std::fill_n(buff, MAXBUFSIZE, 0);
                            break;
                        }

                        std::istringstream linestream(line);

                        while (linestream >> buff[pos])
                        {
                            pos++;
                        }
                    }
                }
                if (line.find("bias") != std::string::npos)
                {
                    while (std::getline(filestream, line))
                    {
                        if (line == "")
                        {
                            Eigen::MatrixXf m = Utils::bufferToMatrix(buff, (*(_layers[layer]->_bias)).rows(), (*(_layers[layer]->_bias)).cols());
                            *(_layers[layer]->_bias) = m;
                            layer++;
                            pos = 0;
                            std::fill_n(buff, MAXBUFSIZE, 0);
                            break;
                        }

                        std::istringstream linestream(line);

                        while (linestream >> buff[pos])
                        {
                            pos++;
                        }
                    }
                }
            }
        }
    }
}

void Model::saveWeights(const std::string &weightsPath)
{
    std::ofstream file(weightsPath);
    if (file.is_open())
    {
        for (int l = 0; l < _layers.size(); l++)
        {
            Eigen::MatrixXf m = *(_layers[l]->_weights);
            file << "layer " << l << "\n"
                 << m << "\n"
                 << std::endl;

            m = *(_layers[l]->_bias);
            file << "bias " << l << "\n"
                 << m << "\n"
                 << std::endl;
        }
    }
}

Eigen::MatrixXf Model::getPredCategories(Eigen::MatrixXf *layerOutput)
{
    Eigen::MatrixXf predictionScores = (*layerOutput).rowwise().maxCoeff();
    Eigen::MatrixXf predictionCategories = Eigen::MatrixXf::Zero(predictionScores.rows(), predictionScores.cols());
    // predictionCategories *= 0;

    for (int i = 0; i < predictionScores.rows(); i++)
    {
        for (int cat = 0; cat < (*layerOutput).cols(); cat++)
        {
            if ((*layerOutput)(i, cat) == predictionScores(i, 0))
            {
                (predictionCategories)(i, 0) = cat;
            }
        }
    }
    return predictionCategories;
}

float Model::accuracy(Eigen::MatrixXf *yPred, Eigen::MatrixXf *yTrue)
{
    float accuracy = 0;
    for (int i = 0; i < (*yTrue).rows(); i++)
    {
        if ((*yTrue)(i, 0) == (*yPred)(i, 0))
            accuracy++;
    }
    return accuracy / (*yTrue).rows();
}

void Model::printModel()
{
    for (int l = 0; l < _layers.size(); l++)
    {
        Layer *layer = _layers[l];
        layer->printLayer();
    }
}

void Model::predict(std::unique_ptr<Eigen::MatrixXf> testX)
{
    std::cout << "*** Model Predict ***\n"
              << std::endl;

    Eigen::MatrixXf *layerOut;
    Eigen::MatrixXf yPred;

    layerOut = testX.get();
    for (int l = 0; l < _layers.size(); l++)
    {
        Layer *layer = _layers[l];
        layer->forward(layerOut);
        layerOut = layer->_output.get();

        Activation *activation = _activationLayers[l];
        activation->forward(layerOut);
        layerOut = activation->_output.get();
    }

    yPred = getPredCategories(layerOut);
    std::cout << "Predictions:\n"
              << yPred << std::endl;
    Utils::plot(testX.get(), &yPred);
}

void Model::test(std::unique_ptr<Eigen::MatrixXf> testX, std::unique_ptr<Eigen::MatrixXf> testY)
{
    std::cout << "*** Model Test ***\n"
              << std::endl;

    Eigen::MatrixXf *layerOut;
    Eigen::MatrixXf yPred;

    layerOut = testX.get();
    for (int l = 0; l < _layers.size(); l++)
    {
        Layer *layer = _layers[l];
        layer->forward(layerOut);
        layerOut = layer->_output.get();

        Activation *activation = _activationLayers[l];
        activation->forward(layerOut);
        layerOut = activation->_output.get();
    }

    yPred = getPredCategories(layerOut);

    std::cout << "Test Accuracy: " << accuracy(&yPred, testY.get()) << "\n"
              << std::endl;
    Utils::plot(testX.get(), &yPred);
}

void Model::train(std::unique_ptr<Eigen::MatrixXf> trainX, std::unique_ptr<Eigen::MatrixXf> trainY)
{
    std::cout << "*** Model Training ***\n"
              << std::endl;

    Eigen::MatrixXf *layerOut;
    Eigen::MatrixXf yPred;
    for (int iter = 0; iter < trainEpochs; iter++)
    {
        layerOut = trainX.get();
        for (int l = 0; l < _layers.size(); l++)
        {
            Layer *layer = _layers[l];
            layer->forward(layerOut);
            layerOut = layer->_output.get();

            Activation *activation = _activationLayers[l];
            activation->forward(layerOut);
            layerOut = activation->_output.get();
        }

        float loss = _loss.forward(layerOut, trainY.get());

        yPred = getPredCategories(layerOut);

        if (iter % 100 == 0)
        {
            std::cout << "Iteration: " << iter << ", Loss: " << loss << ", Accuracy: "
                      << accuracy(&yPred, trainY.get()) << "\n"
                      << std::endl;
            saveWeights(weightsPath);
        }

        _loss.backward(layerOut, trainY.get());
        Eigen::MatrixXf *backpassDeltaValues = _loss._backpassDeltaValues.get();

        for (int l = _layers.size() - 1; l >= 0; l--)
        {
            Activation *activation = _activationLayers[l];
            activation->backward(backpassDeltaValues);
            backpassDeltaValues = activation->_backpassDeltaValues.get();

            Layer *layer = _layers[l];
            layer->backward(backpassDeltaValues);
            backpassDeltaValues = layer->_backpassDeltaValues.get();
        }

        for (int l = 0; l < _layers.size(); l++)
        {
            // Update layer parameters using optimizer
            Layer *layer = _layers[l];
            _optimizer.updateParams(layer);
        }
    }

    Utils::plot(trainX.get(), &yPred);
}