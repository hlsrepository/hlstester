# include <iostream>
# include <Eigen/Dense>

# include "model.h"

using namespace std;

int main(int argc, char *argv[])
{
    std::string mode, config, weights;
    std::string xPath{""}, yPath{""};
    std::unique_ptr<Eigen::MatrixXf> xData;
    std::unique_ptr<Eigen::MatrixXf> yData;

    if (argc < 5)
    {
        std::cout << "Usage: [train/test/pred] [config path] [weights path] <option(s)>\n"
                  << "**Note: Train X and Train Y data must be passed (in that order) if mode is 'train'.\n"
                  << "Test X and Test Y data must be passed if mode is 'test'.\n**"
                  << "Prediction X data must be passed if mode is 'pred'.\n"
                  << "Options:\n"
                  << "    <train X data path if mode is train>\n"
                  << "    <train Y data path if mode is train>\n"
                  << "    <test X data path if mode is test>\n"
                  << "    <pred X data path if mode is pred>\n"
                  << std::endl;
        return 1;
    }

    mode = argv[1];
    config = argv[2];
    weights = argv[3];
    if (mode == "train" || mode == "test")
    {
        if (argc < 6)
        {
            std::cout << "*** Insufficient arguments! ***" << std::endl;
            return 1;
        }
        xPath = argv[4];
        yPath = argv[5];
    }
    else if (mode == "pred")
    {
        xPath = argv[4];
    }
    else
    {
        std::cout << "Unsupported argument in position [1]: " << mode << std::endl;
    }

    if (!xPath.empty())
        xData = std::make_unique<Eigen::MatrixXf>(readCsv<Eigen::MatrixXf>(xPath));

    if (!yPath.empty())
        yData = std::make_unique<Eigen::MatrixXf>(readCsv<Eigen::MatrixXf>(yPath));

    Config networkConfig;
    networkConfig.readConfig(config);
    // networkConfig.printConfig();

    Model model(networkConfig);
    model.printModel();
    model.loadWeights(weights);

    if (mode == "train")
    {
        model.train(std::move(xData), std::move(yData));
        model.saveWeights(weights);
    }
    else if (mode == "test")
    {
        model.test(std::move(xData), std::move(yData));
    }
    else if (mode == "pred")
    {
        model.predict(std::move(xData));
    }

    return 0;
}
