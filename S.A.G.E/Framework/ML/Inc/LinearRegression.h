#pragma once

namespace SAGE::ML
{
	struct Dataset;

	class LinearRegression
	{
	public:
		float Fit(const Dataset& dataset);
		float Predict(float X) const;
		void Reset(float B0 = 0.0f, float B1 = 0.0f, float lR = 0.01f);

		float b0 = 0.0f;
		float b1 = 0.0f;
		float learningRate = 0.01f;
	};

}