#include "Precompiled.h"
#include "LogisticRegression.h"

#include "Datasets.h"

using namespace SAGE::ML;

float LogisticRegression::Fit(const Dataset& dataset)
{
	float db0 = 0.0f;
	float db1 = 0.0f;
	float db2 = 0.0f;
	float mse = 0.0f; // TODO: Fix to use log error functions

	for (size_t i = 0; i < dataset.x0.size(); ++i)
	{
		const float prediction = Predict(dataset.x0[i], dataset.x1[i]);
		const float error = prediction - dataset.y[i];
		db0 += error;
		db1 += error * dataset.x0[i];
		db2 += error * dataset.x1[i];
		mse += (error * error);
	}

	const float m = static_cast<float>(dataset.x0.size());
	b0 -= learningRate * db0 / m;
	b1 -= learningRate * db1 / m;
	b2 -= learningRate * db2 / m;
	return mse / m;
}

float LogisticRegression::Predict(float X0, float X1) const
{
	const float z = b0 + (b1 * X0) + (b2 * X1);
	return 1.0f / (1.0f + exp(-z));
}

void LogisticRegression::Reset(float B0, float B1, float B2, float lR)
{
	b0 = B0;
	b1 = B1;
	b2 = B2;
	learningRate = lR;
}
