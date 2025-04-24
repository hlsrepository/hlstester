# include "utils.h"

void Config::readConfig(std::string configPath)
{
    std::string line, value;
    std::vector<std::string> properties;
    std::ifstream filestream(configPath);
    if (filestream.is_open())
    {
        while (!filestream.eof())
        {
            std::getline(filestream, line);

            if (line.find("[train]") != std::string::npos)
            {
                while (!filestream.eof())
                {
                    std::getline(filestream, line);
                    if (line == "")
                        break;

                    std::vector<std::string> config = Utils::parseProperties(line);
                    trainConfig[config[0]] = config[1];
                }
            }
            else if (line.find("[") != std::string::npos)
            {
                LayerType layer = getLayerType(line);
                layers.push_back(layer);

                while (!filestream.eof())
                {
                    std::getline(filestream, line);
                    if (line == "")
                        break;

                    std::vector<std::string> props = Utils::parseProperties(line);
                    propertiesMap[props[0]] = props[1];
                }

                layerProperties.push_back(propertiesMap);
                properties.clear();
            }
        }
        layerProperties.push_back(propertiesMap);
    }
}

LayerType Config::getLayerType(std::string value)
{
    try
    {
        if (value == "[dense]")
            return LayerType::dense;
        else
            throw "Unrecognized layer type: " + value;
    }
    catch (std::string e)
    {
        std::cout << e << std::endl;
    }
}

bool Utils::fileExists(const std::string &filePath)
{
    if (FILE *file = fopen(filePath.c_str(), "r"))
    {
        fclose(file);
        return true;
    }
    return false;
}

Eigen::MatrixXf Utils::bufferToMatrix(float *buff, int rows, int cols)
{
    Eigen::MatrixXf m(rows, cols);
    for (int row = 0; row < rows; row++)
        for (int col = 0; col < cols; col++)
        {
            m(row, col) = buff[row * cols + col];
        }

    return m;
}

Eigen::MatrixXf Utils::loadMatrix(const std::string &filePath)
{
    int cols = 0, rows = 0;
    float buff[MAXBUFSIZE];

    // // Read numbers from file into buffer.
    std::ifstream filestream(filePath);
    if (filestream.is_open())
    {
        std::string line, token;
        while (std::getline(filestream, line))
        {
            int temp_cols = 0;
            std::stringstream linestream(line);
            std::stringstream stream;

            while (!linestream.eof())
            {
                linestream >> token;
                if (token.find("layer_") != std::string::npos)
                {
                    std::cout << token << std::endl;
                }
                else
                {
                    stream.str(token);
                    stream >> buff[cols * rows + temp_cols++];
                }

                if (temp_cols == 0)
                    continue;

                if (cols == 0)
                    cols = temp_cols;

                rows++;
            }
        }
    }
    rows--;

    // Populate matrix with numbers.
    Eigen::MatrixXf m(rows, cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            m(i, j) = buff[cols * i + j];

    return m;
}

std::vector<std::string> Utils::parseProperties(std::string value)
{
    value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
    return Utils::splitString(value, '=');
}

ActivationFunctionType Utils::parseActivationFunction(std::string value)
{
    try
    {
        if (value.compare("relu") == 0)
        {
            return ActivationFunctionType::relu;
        }
        else if (value.compare("softmax") == 0)
        {
            return ActivationFunctionType::softmax;
        }
        else
            throw "Unrecoknized activation function: " + value;
    }
    catch (std::string e)
    {
        std::cerr << e << std::endl;
    }
}

int Utils::parseInputsOutputs(std::string value)
{
    try
    {
        value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
        std::vector<std::string> splitValue = Utils::splitString(value, '=');
        return std::stoi(splitValue[1]);
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

void Config::printConfig()
{
    std::map<LayerType, std::string> mapLayerType;
    mapLayerType[LayerType::dense] = "DenseLayer";

    std::cout << "Training Configuration:" << std::endl;
    for (auto config : trainConfig)
    {
        std::cout << "    " << config.first << " = " << config.second << std::endl;
    }
    std::cout << std::endl;

    for (int l = 0; l < layers.size(); l++)
    {
        std::cout << "Layer Type:\n    " << mapLayerType[layers[l]] << std::endl;

        std::cout << "Layer Properties:" << std::endl;

        std::map<std::string, std::string> properties = layerProperties[l];
        for (const auto &value : properties)
        {
            std::cout << "    " << value.first << " = " << value.second << std::endl;
        }
        std::cout << std::endl;
    }
}

void Utils::plot(Eigen::MatrixXf *x, Eigen::MatrixXf *y)
{
    std::vector<int> cats((*y).data(), (*y).data() + (*y).rows() * (*y).cols());
    std::vector<std::string> colors{"blue", "gray", "red", "orange", "yellow", "black", "green"};

    std::sort(cats.begin(), cats.end());
    std::vector<int>::iterator it;
    it = std::unique(cats.begin(), cats.end());

    cats.resize(distance(cats.begin(), it));

    for (int cat = 0; cat < cats.size(); cat++)
    {
        std::vector<double> xVec;
        std::vector<double> yVec;
        for (int i = 0; i < x->rows(); ++i)
        {
            if ((*y)(i, 0) == cat)
            {
                xVec.push_back((*x)(i, 0));
                yVec.push_back((*x)(i, 1));
            }
        }
        plt::scatter(xVec, yVec, 10, {{"c", colors[cat]}, {"marker", "o"}});
    }
    plt::show();
}

std::vector<std::string> Utils::splitString(const std::string &s, char delimiter)
{
    std::string token;
    std::vector<std::string> tokens;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}