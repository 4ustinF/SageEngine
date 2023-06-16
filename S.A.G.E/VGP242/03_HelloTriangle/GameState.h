#include <SAGE/Inc/SAGE.h>

class GameState : public SAGE::AppState
{
public:
	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	struct Vertex
	{
		SAGE::Math::Vector3 position;
	};

	std::vector<Vertex> mVertices;

	SAGE::Graphics::MeshBuffer mMeshBuffer;

	SAGE::Graphics::VertexShader mVertexShader;
	SAGE::Graphics::PixelShader mPixelShader;
};