# ifndef LOSS_H
# define LOSS_H

# include <memory>
# include <iostream>
# include <Eigen/Dense>

class Loss
{
public:
    Loss();

    std::unique_ptr<Eigen::MatrixXf> _backpassDeltaValues;
};

class CategoricalCrossEntropy : public Loss
{
public:
    using Loss::Loss;
    void backward(Eigen::MatrixXf *values, Eigen::MatrixXf *yTrue);
    float forward(Eigen::MatrixXf *yPred, Eigen::MatrixXf *yTrue);
};

# endif