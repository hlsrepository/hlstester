# ifndef OPTIMIZER_H
# define OPTIMIZER_H

# include <Eigen/Dense>
# include <mutex>

# include "layer.h"

class StochasticGradientDescent
{
public:
    StochasticGradientDescent(float learningRate);
    StochasticGradientDescent() {}
    void updateParams(Layer *layer);

    std::mutex _mutex;
    float learningRate{1};
};

# endif