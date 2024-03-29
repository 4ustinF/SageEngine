#pragma once

namespace SAGE::ML::NEAT
{
	struct Gene;

	class InnovationContainer
	{
	public:
		void Reset();

		size_t AddGene(const Gene& gene);
		size_t Number() const { }

	private:
		friend class Population;

		void SetInnovationNumber(size_t num)
		{
			mInnovationNumber = num;
			Reset();
		}

		std::map<std::pair<size_t, size_t>, size_t> mHistory;
		size_t mInnovationNumber = 0;
	};
}