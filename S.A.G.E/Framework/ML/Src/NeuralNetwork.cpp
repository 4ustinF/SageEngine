#include "Precompiled.h"
#include "NeuralNetwork.h"

using namespace SAGE::ML;
using namespace SAGE::Math::Random;

namespace
{
	float Activation(float x)
	{
		return tanh(x);
	}

	float ActivationDerivative(float x)
	{
		return 1.0f - (x * x);
	}
}

Neuron::Neuron(size_t numOutputs, size_t myIndex)
	: mMyIndex(myIndex)
{
	for (size_t i = 0; i < numOutputs; ++i)
	{
		mOutputeWeights.push_back({ UniformFloat() });
	}
}

void Neuron::FeedForward(const Layer& previousLayer)
{
	float sum = 0.0f;
	for (size_t neuron = 0; neuron < previousLayer.size(); ++neuron) {
		auto& n = previousLayer[neuron];
		sum += n.GetOutputValue() * n.mOutputeWeights[mMyIndex].weight;
	}

	mOutputValue = Activation(sum);
}

void Neuron::CalculateOutputGradients(float targetValue)
{
	const float error = targetValue - mOutputValue;
	mGradient = error * ActivationDerivative(mOutputValue);
}

void Neuron::CalculateHiddenGradients(const Layer& nextLayer)
{
	float sumDOW = 0.0f;
	for (size_t neuron = 0; neuron + 1 < nextLayer.size(); ++neuron) {
		sumDOW += mOutputeWeights[neuron].weight * nextLayer[neuron].mGradient;
	}
	mGradient = sumDOW * ActivationDerivative(mOutputValue);
}

void Neuron::UpdateInputWeights(Layer& previousLayer)
{
	const float learningRate = 0.15f; // TODO: Should be a value in the neural network itself.

	for (size_t neuron = 0; neuron < previousLayer.size(); ++neuron) {
		auto& n = previousLayer[neuron];
		const float deltaWeight = learningRate * n.GetOutputValue() * mGradient;
		n.mOutputeWeights[mMyIndex].weight += deltaWeight;
	}
}

// ====================================================================================================

NeuralNetwork::NeuralNetwork(const std::vector<size_t>& topology)
{
	const size_t numLayers = topology.size();
	ASSERT(numLayers >= 2, "NeuralNetwork -- Invalid topology. Must have at least 2 layers.");

	mLayers.resize(numLayers);
	for (size_t layer = 0; layer < numLayers; ++layer)
	{
		// Get number of outputs by looking at the neuron count for the next layer.
		// however, the last layer (i.e the output) has no next layer
		const size_t numOutputs = (layer + 1 == numLayers) ? 0 : topology[layer + 1];
		const size_t numNeurons = topology[layer];

		// For each layer, we want 1 extra neuron as the bias neuron (hence <= instead of <)
		for (size_t neuron = 0; neuron <= numNeurons; ++neuron) {
			mLayers[layer].emplace_back(numOutputs, neuron);
		}
	}

	mLayers.back().back().SetOutputValue(1.0f);
}

void NeuralNetwork::FeedForward(const std::vector<float>& inputValues)
{
	ASSERT(inputValues.size() == mLayers.front().size() - 1, "NeuralNetwork -- Invalid input count.");

	// Assign the input values into the input layer neurons
	for (size_t i = 0; i < inputValues.size(); ++i) {
		mLayers.front()[i].SetOutputValue(inputValues[i]);
	}

	// Forward propagate
	for (size_t layer = 0; layer + 1 < mLayers.size(); ++layer) {
		const Layer& currentLayer = mLayers[layer];
		Layer& nextLayer = mLayers[layer + 1];

		for (size_t neuron = 0; neuron + 1 < nextLayer.size(); ++neuron) {
			nextLayer[neuron].FeedForward(currentLayer);
		}
	}
}

void NeuralNetwork::BackPropagate(const std::vector<float>& targetValues)
{
	ASSERT(targetValues.size() == mLayers.back().size() - 1, "NeuralNetwork -- Invalid output count.");

	Layer& outputLayer = mLayers.back();

	// Calculate output layer gradient
	for (size_t neuron = 0; neuron + 1 < outputLayer.size(); ++neuron) {
		outputLayer[neuron].CalculateOutputGradients(targetValues[neuron]);
	}

	// Calculate the gradients on hidden layers
	for (size_t layer = mLayers.size() - 2; layer > 0; --layer) {
		Layer& hiddenLayer = mLayers[layer];
		const Layer& nextLayer = mLayers[layer + 1];

		for (size_t neuron = 0; neuron < hiddenLayer.size(); ++neuron) {
			hiddenLayer[neuron].CalculateHiddenGradients(nextLayer);
		}
	}

	// Update connection weights
	for (size_t layer = mLayers.size() - 1; layer > 0; --layer) {
		Layer& currentLayer = mLayers[layer];
		Layer& previousLayer = mLayers[layer - 1];
		for (size_t neuron = 0; neuron + 1 < currentLayer.size(); ++neuron) {
			currentLayer[neuron].UpdateInputWeights(previousLayer);
		}
	}
}

std::vector<float> NeuralNetwork::GetResults() const
{
	std::vector<float> results;
	const Layer& outputLayer = mLayers.back();
	for (size_t neuron = 0; neuron + 1 < outputLayer.size(); ++neuron) {
		results.push_back(outputLayer[neuron].GetOutputValue());
	}
	return results;
}