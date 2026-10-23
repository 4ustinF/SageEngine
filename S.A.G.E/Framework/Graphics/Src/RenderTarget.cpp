#include "Precompiled.h"
#include "RenderTarget.h"

#include "GraphicsSystem.h"

using namespace SAGE;
using namespace SAGE::Graphics;

RenderTarget::~RenderTarget()
{
	ASSERT(mRenderTargetView == nullptr && mDepthStencilView == nullptr,
		"RenderTarget -- Render target not released!");
}

void RenderTarget::Initialize(const std::filesystem::path& fileName)
{
	ASSERT(false, "RenderTarget -- cannot initialize render target from file!");
}

void RenderTarget::Initialize(uint32_t width, uint32_t height, Format format)
{
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = GetDXGIFormat(format);
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	GraphicsSystem* gs = GraphicsSystem::Get();
	mContext = gs->GetContext();
	auto device = gs->GetDevice();

	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &mColorTexture);
	ASSERT(SUCCEEDED(hr), "RenderTarget -- failed to create render texture");
	hr = device->CreateShaderResourceView(mColorTexture, nullptr, &mShaderResourceView);
	ASSERT(SUCCEEDED(hr), "RenderTarget -- failed to create shader resource view");
	hr = device->CreateRenderTargetView(mColorTexture, nullptr, &mRenderTargetView);
	ASSERT(SUCCEEDED(hr), "RenderTarget -- failed to create render target view");

	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = device->CreateTexture2D(&desc, nullptr, &mDepthTexture);
	ASSERT(SUCCEEDED(hr), "RenderTarget -- failed to create depth stencil texture");
	hr = device->CreateDepthStencilView(mDepthTexture, nullptr, &mDepthStencilView);
	ASSERT(SUCCEEDED(hr), "RenderTarget -- failed to create depth stencil view");

	mWidth = width;   // these were never being set for render targets
	mHeight = height;

	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(width);
	mViewport.Height = static_cast<float>(height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
}

void RenderTarget::Terminate()
{
	mContext = nullptr;
	SafeRelease(mRenderTargetView);
	SafeRelease(mDepthStencilView);
	SafeRelease(mColorTexture);
	SafeRelease(mDepthTexture);
	Texture::Terminate();
}

void RenderTarget::CopyFrom(const RenderTarget& source)
{
	ASSERT(mColorTexture && source.mColorTexture, "RenderTarget -- CopyFrom on uninitialized target");
	ASSERT(mWidth == source.mWidth && mHeight == source.mHeight, "RenderTarget -- CopyFrom size mismatch");

	mContext->CopyResource(mColorTexture, source.mColorTexture);
	mContext->CopyResource(mDepthTexture, source.mDepthTexture);
}

void RenderTarget::BeginRender(Color clearColor, bool clear)
{
	UINT numViewports = 1;
	mContext->OMGetRenderTargets(1, &mOldRenderTargetView, &mOldDepthStencilView);
	mContext->RSGetViewports(&numViewports, &mOldViewport);

	if (clear)
	{
		mContext->ClearRenderTargetView(mRenderTargetView, &clearColor.r);
		mContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	mContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	mContext->RSSetViewports(1, &mViewport);
}

void RenderTarget::EndRender()
{
	mContext->OMSetRenderTargets(1, &mOldRenderTargetView, mOldDepthStencilView);
	mContext->RSSetViewports(1, &mOldViewport);

	// OMGetRenderTargets AddRef'd these. Without this they leak a ref per pass.
	SafeRelease(mOldRenderTargetView);
	SafeRelease(mOldDepthStencilView);
}