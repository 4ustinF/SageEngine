#pragma once

#include "Service.h"

namespace SAGE
{
	class PostProcessingService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::PostProcessing)

		void Initialize() override;
		void Terminate() override;
		void NewRender();
		void DebugUI() override;

		SAGE::Graphics::RenderTarget& GetRenderTarget() { return mRenderTarget; }

	private:
		SAGE::Graphics::PostProcessingEffect mPostProccessingEffect;
		SAGE::Graphics::RenderTarget mRenderTarget;
		SAGE::Graphics::RenderObject mScreenQuad;
	};
}