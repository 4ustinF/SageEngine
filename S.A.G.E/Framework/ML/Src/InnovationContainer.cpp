#include "Precompiled.h"
#include "InnovationContainer.h"

using namespace SAGE::ML::NEAT;

void InnovationContainer::Reset()
{
	// TODO:
}

size_t InnovationContainer::AddGene(const Gene& gene)
{
	// TODO:

	return ++mInnovationNumber;
}
