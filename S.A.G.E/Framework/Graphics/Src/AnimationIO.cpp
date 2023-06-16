#include "Precompiled.h"
#include "AnimationIO.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void AnimationIO::Write(FILE* file, const Animation& animation)
{
	fprintf(file, "Positionkeys: %d\n", static_cast<uint32_t>(animation.mPositionKeys.size()));
	if (!animation.mPositionKeys.empty()) {
		for (auto& poskey : animation.mPositionKeys) {
			fprintf(file, "%f %f %f %f\n", poskey.time, poskey.key.x, poskey.key.y, poskey.key.z);
		}
	}

	fprintf(file, "Rotationkeys: %d\n", static_cast<uint32_t>(animation.mRotationKeys.size()));
	if (!animation.mRotationKeys.empty()) {
		for (auto& rotkey : animation.mRotationKeys) {
			fprintf(file, "%f %f %f %f %f\n", rotkey.time, rotkey.key.w, rotkey.key.x, rotkey.key.y, rotkey.key.z);
		}
	}

	fprintf(file, "Scalekeys: %d\n", static_cast<uint32_t>(animation.mScaleKeys.size()));
	if (!animation.mScaleKeys.empty()) {
		for (auto& scalekey : animation.mScaleKeys) {
			fprintf(file, "%f %f %f %f\n", scalekey.time, scalekey.key.x, scalekey.key.y, scalekey.key.z);
		}
	}
}

void AnimationIO::Read(FILE* file, Animation& animation)
{
	uint32_t positionKeys = 0;
	fscanf_s(file, "Positionkeys: %d\n", &positionKeys);
	if (positionKeys > 0)
	{
		animation.mPositionKeys.resize(positionKeys);
		for (auto& poskey : animation.mPositionKeys) {
			fscanf_s(file, "%f %f %f %f\n", &poskey.time, &poskey.key.x, &poskey.key.y, &poskey.key.z);
		}
	}

	uint32_t rotationKeys = 0;
	fscanf_s(file, "Rotationkeys: %d\n", &rotationKeys);
	if (rotationKeys > 0)
	{
		animation.mRotationKeys.resize(rotationKeys);
		for (auto& rotkey : animation.mRotationKeys) {
			fscanf_s(file, "%f %f %f %f %f\n", &rotkey.time, &rotkey.key.w, &rotkey.key.x, &rotkey.key.y, &rotkey.key.z);
		}
	}

	uint32_t scaleKeys = 0;
	fscanf_s(file, "Scalekeys: %d\n", &scaleKeys);
	if (scaleKeys > 0)
	{
		animation.mScaleKeys.resize(scaleKeys);
		for (auto& scalekey : animation.mScaleKeys) {
			fscanf_s(file, "%f %f %f %f\n", &scalekey.time, &scalekey.key.x, &scalekey.key.y, &scalekey.key.z);
		}
	}
}