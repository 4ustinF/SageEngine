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

		using GeneratePopulationFunc = std::function<void()>;
		using ComputeFitnessFunc = std::function<float(const Genome&)>;
		//using SelectionFunc = std::function<int(std::vector<Genome>)>;
		using MateFunc = std::function<Genome(const Genome& parent1, const Genome& parent2)>;

		void Initialize(GeneratePopulationFunc generatePopulation, ComputeFitnessFunc computeFitness, MateFunc mate);
		void Advance();

		const Genome& GetBestGenome() const { return mPopulation.front(); };
		int Getgeneration() const { return mGeneration; }
		std::vector<Genome>& GetPopulation() { return mPopulation; }

	private:
		void EvaluatePopulation();

		ComputeFitnessFunc mComputeFitnessFunc;
		MateFunc mMateFunc;

		std::vector<Genome> mPopulation;
		int mGeneration = 0;
	};
}