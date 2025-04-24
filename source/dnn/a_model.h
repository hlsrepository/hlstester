# ifndef MODEL_H
# define MODEL_H

# include <memory>
# include <mutex>
# include <Eigen/Dense>

# include "activation.h"
# include "layer.h"
# include "loss.h"
# include "optimizer.h"
# include "utils.h"

# define MAXBUFSIZE ((int)1e6)

class Model
{
public:
    Model(Config config);
    float accuracy(Eigen::MatrixXf *yPred, Eigen::MatrixXf *yTrue);
    Eigen::MatrixXf getPredCategories(Eigen::MatrixXf *layerOutput);
    void loadWeights(const std::string &weightsPath);
    void predictBatch();
    void predictSingle();
    void trainBatch();
    void printModel();
    void predict(std::unique_ptr<Eigen::MatrixXf> testX);
    void saveWeights(const std::string &weightsPath);
    void test(std::unique_ptr<Eigen::MatrixXf> testX, std::unique_ptr<Eigen::MatrixXf> testY);
    void train(std::unique_ptr<Eigen::MatrixXf> trainX, std::unique_ptr<Eigen::MatrixXf> trainY);
    void trainSingle();

    int batchSize{1};
    int trainEpochs{1};
    std::string weightsPath;

private:
    std::vector<Layer *> _layers;
    std::vector<Activation *> _activationLayers;
    CategoricalCrossEntropy _loss;
    StochasticGradientDescent _optimizer;
};

# endif