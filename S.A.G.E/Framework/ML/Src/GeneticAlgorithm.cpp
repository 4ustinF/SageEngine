#include "Precompiled.h"
#include "GeneticAlgorithm.h"

using namespace SAGE::Math;
using namespace SAGE::ML;

void GeneticAlgorithm::Initialize(GeneratePopulationFunc generatePopulation, ComputeFitnessFunc computeFitness, MateFunc mate)
{
	mComputeFitnessFunc = std::move(computeFitness);
	mMateFunc = std::move(mate);

	// Reset
	mGeneration = 0;

	// Create initial population
	mPopulation.clear();
	generatePopulation();
	EvaluatePopulation();
}

void GeneticAlgorithm::Advance()
{
	++mGeneration;

	std::vector<Genome> newpopulation;
	newpopulation.reserve(mPopulation.size());
	
	// Perform elitism: keep top 10%, mate the rest using top 50%
	const int populationSize = static_cast<int>(mPopulation.size());
	const int eliteCount = (populationSize * 10) / 100;
	for (int i = 0; i < eliteCount; ++i) {
		newpopulation.push_back(mPopulation[i]);
	}

	// Mate
	const int mateCount = populationSize - eliteCount;
	const int cutOff = populationSize / 2;
	for (int i = 0; i < mateCount; ++i)
	{
		const auto& parent1 = mPopulation[Random::UniformInt(0, cutOff)];
		const auto& parent2 = mPopulation[Random::UniformInt(0, cutOff)];
		newpopulation.push_back(mMateFunc(parent1, parent2));
	}

	mPopulation = std::move(newpopulation);
	EvaluatePopulation();
}

void GeneticAlgorithm::EvaluatePopulation()
{
	for(auto& genome : mPopulation) {
		genome.fitness = mComputeFitnessFunc(genome);
	}

	std::sort(mPopulation.begin(), mPopulation.end(), [](const auto& a, const auto& b)
		{
			return a.fitness > b.fitness;
		});
}
