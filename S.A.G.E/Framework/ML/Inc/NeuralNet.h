#pragma once
#include "Config.h"

namespace SAGE::ML::NEAT
{
	struct Genome;

	struct Neuron
	{
		enum class Type { Hidden, Input, Output, Bias};

		std::vector<std::pair<size_t, float>> inNodes; // Node index and weight pair
		Type type = Type::Hidden;
		float value = 0.0f;
		bool visited = false;
	};

	class NeuralNet
	{
	public:
		void Initialize(const Genome& genome, const NeuralNetworkConfig& netConfig);

		std::vector<float> Evaluate(const std::vector<float>& input);

	private:
		std::vector<Neuron> mNodes;

		// Indices to the neurons
		std::vector<size_t> mInputNodes;
		std::vector<size_t> mBiasNodes;
		std::vector<size_t> mOutputNodes;
	};
}