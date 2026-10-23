#pragma once

#include "Colors.h"
#include "Texture.h"

namespace SAGE::Graphics
{
	class RenderTarget final : public Texture
	{
	public:
		RenderTarget() = default;
		~RenderTarget() override;

		virtual void Initialize(const std::filesystem::path& fileName) override;
		virtual void Initialize(uint32_t width, uint32_t height, Format format) override;
		virtual void Terminate() override;

		void BeginRender(Color clearColor = Colors::Black, bool clear = true);
		void EndRender();
		void CopyFrom(const RenderTarget& source); // color + depth, must match size/format

	private:
		ID3D11DeviceContext* mContext = nullptr;

		ID3D11Texture2D* mColorTexture = nullptr;
		ID3D11Texture2D* mDepthTexture = nullptr;

		ID3D11RenderTargetView* mRenderTargetView = nullptr;
		ID3D11DepthStencilView* mDepthStencilView = nullptr;
		D3D11_VIEWPORT mViewport{};

		ID3D11RenderTargetView* mOldRenderTargetView = nullptr;
		ID3D11DepthStencilView* mOldDepthStencilView = nullptr;
		D3D11_VIEWPORT mOldViewport{};
	};
}