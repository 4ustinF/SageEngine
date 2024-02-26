#pragma once

namespace SAGE::ML
{
	struct Dataset;

	class KMeans
	{
	public:
		float Fit(const Dataset& dataset);
		size_t Predict(float x0, float x1) const;

		std::vector<std::array<float, 2>> centroids;
		std::vector<size_t> assignments;

		//std::vector<std::vector<SAGE::Math::Vector3>> centroidHistory;
		// TODO:
		// https://scikit-learn.org/stable/modules/generated/sklearn.cluster.KMeans.html
	};
}