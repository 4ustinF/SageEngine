#include "Precompiled.h"
#include "GeneticAlgorithm.h"

using namespace SAGE::Math;
using namespace SAGE::ML;

void GeneticAlgorithm::Initialize(int populationSize, int chromolength, int maxGeneValue, float crossoverRate, float mutationRate, 
	ComputeFitnessFunc computeFitness, MateFunc mate)
{
	mComputeFitnessFunc = std::move(computeFitness);
	mMateFunc = std::move(mate);

	// Reset
	mGeneration = 0;
	mMaxGeneValue = maxGeneValue;
	mCrossoverRate = crossoverRate;
	mMutationRate = mutationRate;

	// Create initial population
	mPopulation.clear();
	mPopulation.resize(populationSize);
	for (auto& genome : mPopulation)
	{
		genome.chromosome.reserve(chromolength);
		for (int i = 0; i < chromolength; ++i) {
			genome.chromosome.push_back(Random::UniformInt(0, maxGeneValue));
		}
	}
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

GeneticAlgorithm::Genome GeneticAlgorithm::Mate(const Genome& parent1, const Genome& parent2)
{
	Genome offSpring;
	offSpring.chromosome.reserve(parent1.chromosome.size());

	for (size_t i = 0; i < parent1.chromosome.size(); ++i)
	{
		if (Random::UniformFloat() < mMutationRate) {
			offSpring.chromosome.push_back(Random::UniformInt(0, mMaxGeneValue));
		}
		else if (Random::UniformFloat() < mCrossoverRate) {
			offSpring.chromosome.push_back(parent1.chromosome[i]);
		}
		else {
			offSpring.chromosome.push_back(parent2.chromosome[i]);
		}
	}

	return offSpring;
}


// TODO:
// Allow crossover/mutate to be std::functions and use GA for pathfinding 