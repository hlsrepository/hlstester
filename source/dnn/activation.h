# ifndef ACTIVATION_H
# define ACTIVATION_H

# include <memory>
# include <iostream>
# include <Eigen/Dense>

enum ActivationFunctionType
{
    relu,
    softmax
};

class Activation
{
public:
    Activation();
    virtual ~Activation();
    virtual void forward(Eigen::MatrixXf *m) {}
    virtual void backward(Eigen::MatrixXf *m) {}

    std::unique_ptr<Eigen::MatrixXf> _input;
    std::unique_ptr<Eigen::MatrixXf> _output;
    std::unique_ptr<Eigen::MatrixXf> _backpassDeltaValues;
};

class Relu : public Activation
{
public:
    using Activation::Activation;
    void forward(Eigen::MatrixXf *m);
    void backward(Eigen::MatrixXf *m);
};

class Softmax : public Activation
{
public:
    using Activation::Activation;
    void forward(Eigen::MatrixXf *m);
    void backward(Eigen::MatrixXf *m);
};

# endif