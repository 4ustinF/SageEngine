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
	std::vector<SAGE::Graphics::VertexPC> mVertices;

	SAGE::Graphics::MeshBuffer mMeshBuffer;

	SAGE::Graphics::VertexShader mVertexShader;
	SAGE::Graphics::PixelShader mPixelShader;
};