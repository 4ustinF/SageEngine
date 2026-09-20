#pragma once

#include <Math/Inc/SAGEMath.h>
#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"

namespace SAGE::Graphics
{
	struct SpotShadowTransformData
	{
		SAGE::Math::Matrix4 wvp;
	};

	struct SpotShadowBoneTransformData
	{
		static constexpr size_t MaxBoneCount = 128;
		Math::Matrix4 boneTransforms[MaxBoneCount];
	};

	struct SpotShadowSettingsData
	{
		int useSkinning = 0;
		float padding[3] = {};
	};

	class SpotShadowEffectResources
	{
	public:
		void Initialize();
		void Terminate();

		using TransformBuffer = TypedConstantBuffer<SpotShadowTransformData>;
		using BoneTransformBuffer = TypedConstantBuffer<SpotShadowBoneTransformData>;
		using SettingsBuffer = TypedConstantBuffer<SpotShadowSettingsData>;

		VertexShader vertexShader;
		PixelShader pixelShader;

		TransformBuffer transformBuffer;
		BoneTransformBuffer boneTransformBuffer;
		SettingsBuffer settingsBuffer;
	 };
}