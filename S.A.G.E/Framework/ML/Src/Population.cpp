#include "Precompiled.h"
#include "Population.h"

using namespace SAGE::ML::NEAT;
using namespace SAGE::Math;

Population::Population(size_t input, size_t output, size_t bias)
{
	neuralNetConfig.input_size = input;
	neuralNetConfig.output_size = output;
	neuralNetConfig.bias_size = bias;
	neuralNetConfig.functional_nodes = input + output + bias;

	// Create a basic generation with default genomes
	for (size_t i = 0; i < speciatingConfig.population; ++i) {
		AddToSpecies(MakeFirstGenome());
	}
}

void Population::NewGeneration()
{
	// mInnovation.Reset();
	CullSpecies(false);
	RankGlobally();
	RemoveStaleSpecies();

	for (auto& s : species) {
		CalculateAverageFitness(s);
	}
	RemoveWeakSpecies();

	std::vector<Genome> children;
	const size_t sum = TotalAverageFitness();
	for (auto& s : species) {
		const size_t breed = (size_t)std::floor(1.0 * s.average_fitness / sum * speciatingConfig.population) - 1;
		for (size_t i = 0; i < breed; ++i) {
			children.push_back(BreedChild(s));
		}
	}

	CullSpecies(true); // Now in each species we have only one genome

	// Preparing for making babies <3
	std::vector<Species*> species_pointer;
	for (auto& s : species) {
		species_pointer.push_back(&s);
	}

	while (children.size() + species.size() < speciatingConfig.population) {
		const size_t speciesIndex = static_cast<size_t>(Random::UniformInt(0, static_cast<int>(species.size()) - 1));
		children.push_back(BreedChild(*species_pointer[speciesIndex]));
	}

	for (auto& genome : children) {
		AddToSpecies(genome);
	}
	++mGenerationNumber;
}

Genome Population::MakeFirstGenome()
{
	Genome genome(neuralNetConfig.functional_nodes);

	for (size_t i = 0; i < neuralNetConfig.input_size; ++i) {
		for (size_t o = 0; o < neuralNetConfig.output_size; ++o) {
			Gene gene;
			gene.fromNode = i;
			gene.toNode = neuralNetConfig.input_size + neuralNetConfig.bias_size + o;
			gene.innovationNum = mInnovation.AddGene(gene);
			gene.weight = Random::UniformFloat(-2.0f, 2.0f);
			genome.genes[gene.innovationNum] = gene;
		}
	}

	for (size_t b = 0; b < neuralNetConfig.bias_size; ++b) {
		for (size_t o = 0; o < neuralNetConfig.output_size; ++o) {
			Gene gene;
			gene.fromNode = neuralNetConfig.input_size + b;
			gene.toNode = neuralNetConfig.input_size + neuralNetConfig.bias_size + o;
			gene.innovationNum = mInnovation.AddGene(gene);
			gene.weight = Random::UniformFloat(-2.0f, 2.0f);
			genome.genes[gene.innovationNum] = gene;
		}
	}

	return genome;
}

Genome Population::Crossover(const Genome& g1, const Genome& g2)
{
	// Make sure g1 has the higher fitness
	if (g2.fitness > g1.fitness) {
		return Crossover(g2, g1);
	}

	Genome child(g1.maxNeuron);

	for (const auto& [innovNum, gene] : g1.genes) {
		const auto it2 = g2.genes.find(innovNum);
		if (it2 != g2.genes.end() && Random::UniformInt(0, 1) == 0) {
			child.genes[innovNum] = it2->second;
		}
		else {
			child.genes[innovNum] = gene;
		}
	}

	return child;
}

void Population::MutateWeight(Genome& g)
{
	const float step = mutationConfig.step_size;
	for (auto& [innovNum, gene] : g.genes) {
		if (Random::UniformFloat() < mutationConfig.perturb_chance) {
			gene.weight += Random::UniformFloat(-step, step);
		}
		else {
			gene.weight = Random::UniformFloat(-2.0f, 2.0f);
		}
	}
}

void Population::MutateEnableDisable(Genome& g, bool enable)
{
	std::vector<Gene*> v;

	// Find all nodes that are not 'enable'
	for (auto& [innovNum, gene] : g.genes) {
		if (gene.enabled != enable) {
			v.push_back(&gene);
		}
	}

	// Randomly pick one of them and set enable flag
	if (!v.empty()) {
		const size_t index = Random::UniformInt(0, static_cast<int>(v.size()) - 1);
		v[index]->enabled = enable;
	}
}

void Population::MutateLink(Genome& g, bool force_bias)
{
	// Network encoding:
	// | input nodes | bias | output nodes |

	auto is_input = [&](size_t node) -> bool
		{
			return node < neuralNetConfig.input_size;
		};
	auto is_output = [&](size_t node) -> bool
		{
			return node < neuralNetConfig.functional_nodes && node >= (neuralNetConfig.input_size + neuralNetConfig.bias_size);
		};
	auto is_bias = [&](size_t node) -> bool
		{
			return node < (neuralNetConfig.input_size + neuralNetConfig.bias_size) && node >= neuralNetConfig.input_size;
		};

	size_t neuron1 = static_cast<size_t>(Random::UniformInt(0, static_cast<int>(g.maxNeuron) - 1));
	size_t neuron2 = static_cast<size_t>(Random::UniformInt(static_cast<int>(neuralNetConfig.input_size + neuralNetConfig.bias_size), static_cast<int>(g.maxNeuron) - 1));

	if (is_output(neuron1) && is_output(neuron2)) {
		return;
	}
	if (is_bias(neuron2)) {
		return;
	}
	if (neuron1 == neuron2 && (!force_bias)) {
		return;
	}
	if (is_output(neuron1)) {
		std::swap(neuron1, neuron2);
	}

	if (force_bias) {
		neuron1 = static_cast<size_t>(Random::UniformInt(static_cast<int>(neuralNetConfig.input_size), 
			static_cast<int>(neuralNetConfig.input_size + neuralNetConfig.bias_size) - 1));
	}

	// Check for recurrency using BFS
	if (!is_bias(neuron1) && !is_input(neuron1))
	{
		bool has_recurrence = false;

		std::queue<size_t> que;
		std::vector<std::vector<size_t>> connections(g.maxNeuron);

		for(auto& [innovNum, gene] : g.genes) {
			connections[gene.fromNode].push_back(gene.toNode);
		}

		connections[neuron1].push_back(neuron2);

		for (size_t i = 0; i < connections[neuron1].size(); ++i) {
			que.push(connections[neuron1][i]);
		}

		while (!que.empty())
		{
			size_t tmp = que.front();
			if (tmp == neuron1)
			{
				has_recurrence = true;
				break;
			}
			que.pop();
			for (size_t i = 0; i < connections[tmp].size(); ++i) {
				que.push(connections[tmp][i]);
			}
		}

		if (has_recurrence) {
			return;
		}
	}

	// If genome already has this connection
	for (auto& [innovNum, gene] : g.genes) {
		if (gene.fromNode == neuron1 && gene.toNode == neuron2) {
			return;
		}
	}

	// Add new innovation if needed
	Gene new_gene;
	new_gene.fromNode = neuron1;
	new_gene.toNode = neuron2;
	new_gene.innovationNum = mInnovation.AddGene(new_gene);
	new_gene.weight = Random::UniformFloat(-2.0f, 2.0f);
	g.genes[new_gene.innovationNum] = new_gene;
}

void Population::MutateNode(Genome& g)
{
}

void Population::Mutate(Genome& g)
{
}

float Population::Disjoint(const Genome& g1, const Genome& g2)
{
	return 0.0f;
}

float Population::Weights(const Genome& g1, const Genome& g2)
{
	return 0.0f;
}

bool Population::IsSameSpecies(const Genome& g1, const Genome& g2)
{
	return false;
}

void Population::RankGlobally()
{
}

void Population::CalculateAverageFitness(Species& s)
{
}

size_t Population::TotalAverageFitness() const
{
	return size_t();
}

void Population::CullSpecies(bool cut_to_one)
{
}

Genome Population::BreedChild(const Species& s)
{
	return Genome();
}

void Population::RemoveStaleSpecies()
{
}

void Population::RemoveWeakSpecies()
{
}

void Population::AddToSpecies(const genome& child)
{
}
