#include "Precompiled.h"
#include "Population.h"

#include <queue>

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
		const size_t speciesIndex = Random::UniformSizet(0, static_cast<int>(species.size()) - 1);
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

	size_t neuron1 = Random::UniformSizet(0, static_cast<int>(g.maxNeuron) - 1);
	size_t neuron2 = Random::UniformSizet(static_cast<int>(neuralNetConfig.input_size + neuralNetConfig.bias_size), static_cast<int>(g.maxNeuron) - 1);

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
		neuron1 = Random::UniformSizet(static_cast<int>(neuralNetConfig.input_size), 
			static_cast<int>(neuralNetConfig.input_size + neuralNetConfig.bias_size) - 1);
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
	if (g.genes.empty()) {
		return;
	}

	g.maxNeuron++;

	// Randomly choose a gene to mutate
	const size_t gene_id = Random::UniformSizet(0, static_cast<int>(g.genes.size() - 1));
	auto it = g.genes.begin();
	std::advance(it, gene_id);

	auto& gene = it->second;
	if (!gene.enabled) {
		return;
	}

	gene.enabled = false;

	Gene new_gene1;
	new_gene1.fromNode = gene.fromNode;
	new_gene1.toNode = g.maxNeuron - 1; // To the last created neuron.
	new_gene1.weight = 1.0f;
	new_gene1.innovationNum = mInnovation.AddGene(new_gene1);
	new_gene1.enabled = true;

	Gene new_gene2;
	new_gene2.fromNode = g.maxNeuron - 1; // To the last created neuron.
	new_gene2.toNode = gene.toNode;
	new_gene2.weight = gene.weight;
	new_gene2.innovationNum = mInnovation.AddGene(new_gene2);
	new_gene2.enabled = true;

	g.genes[new_gene1.innovationNum] = new_gene1;
	g.genes[new_gene2.innovationNum] = new_gene2;
}

void Population::Mutate(Genome& g)
{
	if (Random::UniformFloat() < mutationConfig.connection_mutate_chance) {
		MutateWeight(g);
	}

	if (Random::UniformFloat() < mutationConfig.link_mutation_chance) {
		MutateLink(g, false);
	}

	if (Random::UniformFloat() < mutationConfig.bias_mutation_chance) {
		MutateLink(g, true);
	}

	if (Random::UniformFloat() < mutationConfig.node_mutation_chance) {
		MutateNode(g);
	}

	if (Random::UniformFloat() < mutationConfig.enable_mutation_chance) {
		MutateEnableDisable(g, true);
	}

	if (Random::UniformFloat() < mutationConfig.disable_mutation_chance) {
		MutateEnableDisable(g, false);
	}
}

float Population::Disjoint(const Genome& g1, const Genome& g2)
{
	size_t disjoint_count = 0;

	for (const auto& [innovNum, gene] : g1.genes) {
		if (g2.genes.find(innovNum) == g2.genes.end()) {
			++disjoint_count;
		}
	}

	for (const auto& [innovNum, gene] : g2.genes) {
		if (g1.genes.find(innovNum) == g1.genes.end()) {
			++disjoint_count;
		}
	}

	return static_cast<float>(disjoint_count) / std::max(g1.genes.size(), g2.genes.size());
}

float Population::Weights(const Genome& g1, const Genome& g2)
{
	float sum = 0.0f;
	size_t numMatch = 0;

	for (const auto& [innovNum, gene] : g1.genes) {
		auto it2 = g2.genes.find(innovNum);
		if (it2 != g2.genes.end()) {
			sum += std::abs(gene.weight - it2->second.weight);
			++numMatch;
		}
	}

	return sum / numMatch;
}

bool Population::IsSameSpecies(const Genome& g1, const Genome& g2)
{
	const float dd = speciatingConfig.delta_disjoint * Disjoint(g1, g2);
	const float dw = speciatingConfig.delta_weights * Weights(g1, g2);
	return dd + dw < speciatingConfig.delta_threshold;
}

void Population::RankGlobally()
{
	std::vector<Genome*> global;

	for (auto& s : species) {
		for (auto& g : s.genomes) {
			global.push_back(&g);
		}
	}

	std::sort(global.begin(), global.end(), [](auto a, auto b) { return a->fitness < b->fitness; });

	for (size_t j = 0; j < global.size(); ++j) {
		global[j]->globalRank = j + 1;
	}
}

void Population::CalculateAverageFitness(Species& s)
{
	size_t total = 0;
	for (auto& g : s.genomes) {
		total += g.globalRank;
	}
	s.average_fitness = total / s.genomes.size();
}

size_t Population::TotalAverageFitness() const
{
	size_t total = 0;
	for (const auto& s : species) {
		total += s.average_fitness;
	}
	return total;
}

void Population::CullSpecies(bool cut_to_one)
{
	for (auto& s : species)
	{
		std::sort(s.genomes.begin(), s.genomes.end(), [](auto a, auto b) { return a->fitness > b->fitness; });

		const size_t numToKeep = cut_to_one ? 1 :
			static_cast<size_t>(std::ceil(s.genomes.size() / 2.0));

		while (s.genomes.size() > numToKeep) {
			s.genomes.pop_back();
		}
	}
}

Genome Population::BreedChild(const Species& s)
{
	Genome child(0);

	const size_t parent1 = Random::UniformSizet(0, static_cast<int>(s.genomes.size()) - 1);
	if (Random::UniformFloat() < mutationConfig.crossover_chance) {
		const size_t parent2 = Random::UniformSizet(0, static_cast<int>(s.genomes.size()) - 1);
		const Genome& g1 = s.genomes[parent1];
		const Genome& g2 = s.genomes[parent2];
		child = Crossover(g1, g2);
	}
	else {
		child = s.genomes[parent1];
	}

	Mutate(child);
	return child;
}

void Population::RemoveStaleSpecies()
{
	// Update top fitness of each species
	for (auto& s : species)
	{
		auto g = std::max_element(s.genomes.begin(), s.genomes.end(), [](auto& a, auto& b) {
			return a.fitness < b.fitness;
			});

		if (g->fitness > s.top_fitness) {
			s.top_fitness = g->fitness;
			s.staleness = 0;
		}
		else {
			++s.staleness;
		}
	}

	// Prune any species that have not improved for some time
	// Don't remove the last species!
	species.remove_if([this](auto& s) {
		return species.size() > 1 && s.staleness > speciatingConfig.stale_species;
		});
}

void Population::RemoveWeakSpecies()
{
	const size_t sum = TotalAverageFitness();

	auto backup = species;
	backup.remove_if([this, sum](auto& s) {
		return 1.0 > std::floor(1.0 * s.average_fitness / sum * speciatingConfig.population);
		});

	ASSERT(!backup.empty(), "Population --- Failed to backup");
	species = backup;
}

void Population::AddToSpecies(const Genome& child)
{
	for (auto& s : species) {
		if (IsSameSpecies(child, s.genomes[0])) {
			s.genomes.push_back(child);
			return;
		}
	}

	Species newSpecies;
	newSpecies.genomes.push_back(child);
	species.push_back(newSpecies);
}
