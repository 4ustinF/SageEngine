#include "Precompiled.h"
#include "LinearRegression.h"

#include "Datasets.h"

using namespace SAGE::ML;

float LinearRegression::Fit(const Dataset& dataset)
{
	float db0 = 0.0f;
	float db1 = 0.0f;
	float mse = 0.0f;

	for (size_t i = 0; i < dataset.x0.size(); ++i)
	{
		const float prediction = Predict(dataset.x0[i]);
		const float error = Predict(dataset.y[i]);
		db0 += error;
		db1 += error * dataset.x0[i];
		mse += (error * error);
	}

	const float m = static_cast<float>(dataset.x0.size());
	b0 -= learningRate * db0 / m;
	b1 -= learningRate * db1 / m;
	return mse / m;
}

float LinearRegression::Predict(float X) const
{
	return b0 + (b1 * X);
}
