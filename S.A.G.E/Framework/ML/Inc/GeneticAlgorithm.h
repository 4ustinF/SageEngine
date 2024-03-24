#pragma once

namespace SAGE::ML 
{
	class GeneticAlgorithm
	{
	public:
		// Genome -> chromosomes -> genes
		struct Genome
		{
			std::vector<int> chromosome; // [gene][gene][gene][gene]...
			float fitness = 0.0f;
		};

		using ComputeFitnessFunc = std::function<float(const Genome&)>;
		//using SelectionFunc = std::function<int(std::vector<Genome>)>;

		void Initialize(int populationSize, int chromolength, int maxGeneValue, float crossoverRate, float mutationRate, ComputeFitnessFunc computeFitness);
		void Advance();

		const Genome& GetBestGenome() const { return mPopulation.front(); };
		int Getgeneration() const { return mGeneration; }

	private:
		void EvaluatePopulation();
		Genome Mate(const Genome& parent1, const Genome& parent2);

		ComputeFitnessFunc mComputeFitnessFunc;

		std::vector<Genome> mPopulation;
		int mGeneration = 0;
		int mMaxGeneValue = 0;

		float mCrossoverRate = 0.0f;
		float mMutationRate = 0.0f;
	};
}