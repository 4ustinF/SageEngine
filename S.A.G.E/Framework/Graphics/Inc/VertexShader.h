#pragma once

namespace SAGE::Graphics
{
	class VertexShader final
	{
	public:
		template <class VertexTypes>
		void Initialize(const std::filesystem::path& filePath) 
		{
			Initialize(filePath, VertexTypes::Format);
		}

		void Initialize(const std::filesystem::path& filePath, uint32_t format);
		void Terminate();

		void Bind();

	private:
		ID3D11VertexShader* mVertexShader = nullptr;
		ID3D11InputLayout* mInputLayout = nullptr;
	};
}