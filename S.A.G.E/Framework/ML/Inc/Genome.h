#pragma once

namespace SAGE::ML::NEAT
{
	struct Gene
	{
		size_t innovationNum = -1;
		size_t fromNode = -1;
		size_t toNode = -1;
		float weight = 0.0f;
		bool enabled = true;
	};

	struct Genome
	{
	public:
		size_t fitness = 0;
		size_t adjustedFitness = 0;
		size_t globalRank= 0;
		size_t maxNeuron = 0;

		std::map<size_t, Gene> genes;

		Genome(size_t functional_nodes)
			: maxNeuron(functional_nodes)
		{
		}
	};
}