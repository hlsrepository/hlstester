# ifndef UTILS_H
# define UTILS_H

# include <algorithm>
# include <iostream>
# include <set>
# include <string>
# include <vector>
# include <fstream>
# include <regex>
# include <Eigen/Dense>
# include "matplotlibcpp.h"

# include "activation.h"
# include "layer.h"

# define MAXBUFSIZE ((int)1e6)
namespace plt = matplotlibcpp;

class Config
{
public:
    LayerType getLayerType(std::string key);
    void printConfig();
    void readConfig(std::string configPath);

    std::vector<LayerType> layers;
    std::vector<std::map<std::string, std::string>> layerProperties;
    std::map<std::string, std::string> propertiesMap;
    std::map<std::string, std::string> trainConfig;

private:
};

namespace Utils
{
Eigen::MatrixXf bufferToMatrix(float *buff, int rows, int cols);
bool fileExists(const std::string &filePath);
Eigen::MatrixXf loadMatrix(const std::string &filePath);
ActivationFunctionType parseActivationFunction(std::string value);
int parseInputsOutputs(std::string value);
std::vector<std::string> parseProperties(std::string value);
void plot(Eigen::MatrixXf *x, Eigen::MatrixXf *y);
std::vector<std::string> splitString(const std::string &s, char delimiter);
}; // namespace Utils

template <typename M>
M readCsv(const std::string &path)
{
    std::ifstream indata;
    indata.open(path);
    std::string line;
    std::vector<float> values;
    uint rows = 0;
    while (std::getline(indata, line))
    {
        std::stringstream lineStream(line);
        std::string cell;
        while (std::getline(lineStream, cell, ','))
        {
            values.push_back(std::stod(cell));
        }
        ++rows;
    }
    return Eigen::Map<const Eigen::Matrix<typename M::Scalar, M::RowsAtCompileTime, M::ColsAtCompileTime, Eigen::RowMajor>>(values.data(), rows, values.size() / rows);
}

# endif