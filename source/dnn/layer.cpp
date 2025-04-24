# include "layer.h"

Layer::Layer(int inputs, int outputs, int batchSize, ActivationFunctionType activation)
	: _inputs(inputs), _outputs(outputs), batchSize(batchSize), _activation(activation)
{
	// Randomize weights matrix initialization
	// srand((unsigned int)time(0));

	_input = std::make_unique<Eigen::MatrixXf>();
	_output = std::make_unique<Eigen::MatrixXf>();
	_weights = std::make_unique<Eigen::MatrixXf>(Eigen::MatrixXf::Random(inputs, outputs));
	*_weights *= 0.01;
	_weightsDelta = std::make_unique<Eigen::MatrixXf>();
	_backpassDeltaValues = std::make_unique<Eigen::MatrixXf>();
	_bias = std::make_unique<Eigen::MatrixXf>(Eigen::MatrixXf::Zero(batchSize, outputs));
	_biasDelta = std::make_unique<Eigen::MatrixXf>(Eigen::MatrixXf::Zero(batchSize, outputs));
}

void Layer::printLayer()
{
	std::map<ActivationFunctionType, std::string> mapActivationType;
	mapActivationType[ActivationFunctionType::relu] = "relu";
	mapActivationType[ActivationFunctionType::softmax] = "softmax";
	Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");

	std::cout << "Layer Type:\n    Layer" << std::endl;
	std::cout << "Layer Properties:"
			  << "\n    Inputs: " << _inputs
			  << "\n    Outputs: " << _outputs
			  << "\n    Activation: " << mapActivationType[_activation]
			  << "\n    Weights:\n"
			  << (*_weights).format(CleanFmt)
			  << "\n    Bias: " << *_bias
			  << "\n"
			  << std::endl;
}

void Layer::forward(Eigen::MatrixXf *m)
{
	// Save input values
	*_input = *m;

	// Calculate forward pass
	*_output = *m * *_weights;

	for (int row = 0; row < (*_output).rows(); row++)
	{
		for (int col = 0; col < (*_output).cols(); col++)
		{
			(*_output)(row, col) += (*_bias)(0, col);
		}
	}
}

void Layer::backward(Eigen::MatrixXf *m)
{
	// Calculate gradient of the weights
	*_weightsDelta = (*_input).transpose() * *m;

	*_biasDelta = (*m).colwise().sum();

	*_backpassDeltaValues = *m * (*_weights).transpose();
}

DenseLayer::DenseLayer(int inputs, int outputs, int batchSize, ActivationFunctionType activation, float dropout = 0.0)
	: Layer(inputs, outputs, batchSize, activation), _dropoutRate(dropout)
{
}

void DenseLayer::printLayer()
{
	std::map<ActivationFunctionType, std::string> mapActivationType;
	mapActivationType[ActivationFunctionType::relu] = "relu";
	mapActivationType[ActivationFunctionType::softmax] = "softmax";
	Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");

	std::cout << "Layer Type:\n    Dense Layer" << std::endl;
	std::cout << "Layer Properties:"
			  << "\n    Inputs: " << _inputs
			  << "\n    Outputs: " << _outputs
			  << "\n    Activation: " << mapActivationType[_activation]
			  << "\n"
			  << std::endl;
}