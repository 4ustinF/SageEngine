#pragma once
#include <d3d11.h>

namespace SAGE::Graphics::TextureBaking
{
	bool SaveSRVToDDS(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv, const wchar_t* filepath);
	ID3D11ShaderResourceView* LoadDDSAsSRV(ID3D11Device* device, const wchar_t* filepath);
}