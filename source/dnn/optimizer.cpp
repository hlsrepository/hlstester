# include "optimizer.h"

StochasticGradientDescent::StochasticGradientDescent(float learningRate)
{
    learningRate = learningRate;
}

void StochasticGradientDescent::updateParams(Layer *layer)
{
    // lock resources before modifying
    std::unique_lock<std::mutex> lck(_mutex);

    // update weights and bias
    *(layer->_weights) -= (*(layer->_weightsDelta) * learningRate);
    *(layer->_bias) -= (*(layer->_biasDelta) * learningRate);

    lck.unlock();
}