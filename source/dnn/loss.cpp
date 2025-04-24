# include "loss.h"

Loss::Loss()
{
    _backpassDeltaValues = std::make_unique<Eigen::MatrixXf>();
}

float CategoricalCrossEntropy::forward(Eigen::MatrixXf *yPred, Eigen::MatrixXf *yTrue)
{
    // copy of yPred
    Eigen::MatrixXf yPrediction = *yPred;
    // number of training samples
    int numSamples = yPrediction.rows();
    // For categorical labels, calculate probabilities
    if ((*yTrue).cols() == 1)
    {
        Eigen::MatrixXf yPredArray = Eigen::MatrixXf::Zero(numSamples, 1);
        int yTrueCat = 0;
        for (int r = 0; r < numSamples; r++)
        {
            yTrueCat = (*yTrue)(r, 0);
            yPredArray(r, 0) = yPrediction(r, yTrueCat);
        }
        yPrediction = yPredArray;
    }

    // Calculate losses
    Eigen::MatrixXf negLogLikelihoods = yPrediction.array().log() * -1;

    // For on-hot-encoded labels, mask labels with likelihoods
    if ((*yTrue).cols() == 2)
    {
        negLogLikelihoods *= *yTrue;
    }

    // Return overall loss of training data
    return negLogLikelihoods.sum() / numSamples;
}

void CategoricalCrossEntropy::backward(Eigen::MatrixXf *values, Eigen::MatrixXf *yTrue)
{
    // number of training samples
    int numSamples = (*yTrue).rows();
    int yTrueCat = 0;
    *_backpassDeltaValues = *values;

    for (int r = 0; r < numSamples; r++)
    {
        yTrueCat = (*yTrue)(r, 0);
        (*_backpassDeltaValues)(r, yTrueCat) -= 1;
    }
    *_backpassDeltaValues /= (float)numSamples;
}