#pragma once

namespace SAGE::ML::NEAT
{
	struct MutationConfig
	{
		float connection_mutate_chance = 0.65f;
		float perturb_chance = 0.9f;
		float crossover_chance = 0.75f;
		float link_mutation_chance = 0.85f;
		float node_mutation_chance = 0.5f;
		float bias_mutation_chance = 0.2f;
		float step_size = 0.1f;
		float disable_mutation_chance = 0.2f;
		float enable_mutation_chance = 0.2f;
	};

	struct SpeciatingConfig
	{
		size_t population = 300;
		float delta_disjoint = 2.0f;
		float delta_weights = 0.4f;
		float delta_threshold = 1.3f;
		size_t stale_species = 15;
	};

	struct NeuralNetConfig
	{
		size_t input_size;
		size_t bias_size;
		size_t output_size;
		size_t functional_nodes;
	};
}