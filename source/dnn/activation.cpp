# include "activation.h"

Activation::Activation()
{
    _input = std::make_unique<Eigen::MatrixXf>();
    _output = std::make_unique<Eigen::MatrixXf>();
    _backpassDeltaValues = std::make_unique<Eigen::MatrixXf>();
}

Activation::~Activation()
{
}

void Relu::forward(Eigen::MatrixXf *m)
{
    *_input = *m;
    *_output = Eigen::MatrixXf::Zero((*m).rows(), (*m).cols());
    for (int i = 0; i < (*m).rows(); i++)
    {
        for (int j = 0; j < (*m).cols(); j++)
        {
            if ((*m)(i, j) > 0)
                (*_output)(i, j) = (*m)(i, j);
        }
    }
}

void Relu::backward(Eigen::MatrixXf *m)
{
    *_backpassDeltaValues = *m;

    for (int i = 0; i < (*m).rows(); i++)
    {
        for (int j = 0; j < (*m).cols(); j++)
        {
            if ((*_input)(i, j) <= 0)
            {
                (*_backpassDeltaValues)(i, j) = 0;
            }
        }
    }
}

void Softmax::forward(Eigen::MatrixXf *m)
{
    *_input = *m;
    *_output = Eigen::MatrixXf::Zero((*m).rows(), (*m).cols());

    Eigen::MatrixXf maxValues = (*m).rowwise().maxCoeff();
    // std::cout << "max values:\n"
    //           << maxValues << std::endl;

    std::unique_ptr<Eigen::MatrixXf> expValues = std::make_unique<Eigen::MatrixXf>(*m);
    for (int row = 0; row < (*m).rows(); row++)
    {
        (*expValues)(row, Eigen::all) = (*expValues)(row, Eigen::all).array() - maxValues(row, 0);
    }
    // std::cout << "diff values:\n"
    //           << diffValues << std::endl;

    (*expValues) = (*expValues).array().exp();
    // std::cout << "exp values:\n"
    //           << expValues << std::endl;

    // Eigen::MatrixXf expValuesRowSum = (*expValues).rowwise().sum();
    // std::cout << "row sum:\n"
    //           << expValuesRowSum
    //           << std::endl;

    for (int row = 0; row < (*expValues).rows(); row++)
    {
        for (int col = 0; col < (*expValues).cols(); col++)
        {
            (*_output)(row, col) = (*expValues)(row, col) / (*expValues)(row, Eigen::all).sum(); //expValuesRowSum(row);
        }
    }
    // std::cout << "softmax output:\n"
    //           << (*_output) << "\n"
    //           << std::endl;
}

void Softmax::backward(Eigen::MatrixXf *m)
{
    *_backpassDeltaValues = *m;
}