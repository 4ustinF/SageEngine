#include "Precompiled.h"
#include "TextureBaking.h"

#include <DirectXTK/Inc/ScreenGrab.h>
#include <DirectXTK/Inc/DDSTextureLoader.h>

using namespace SAGE;
using namespace SAGE::Graphics;

bool TextureBaking::SaveSRVToDDS(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv, const wchar_t* filepath)
{
    ID3D11Resource* resource = nullptr;
    srv->GetResource(&resource);

    HRESULT hr = DirectX::SaveDDSTextureToFile(context, resource, filepath);

    resource->Release();
    return SUCCEEDED(hr);
}

ID3D11ShaderResourceView* TextureBaking::LoadDDSAsSRV(ID3D11Device* device, const wchar_t* filepath)
{
    ID3D11ShaderResourceView* srv = nullptr;
    HRESULT hr = DirectX::CreateDDSTextureFromFile(device, filepath, nullptr, &srv);
    return FAILED(hr) ? nullptr : srv;
}