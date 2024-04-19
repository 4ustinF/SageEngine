#include "PipeComponent.h"

using namespace SAGE;
using namespace SAGE::Math;

MEMORY_POOL_DEFINE(PipeComponent, 300);

void PipeComponent::Initialize()
{
	Reset();
}

void PipeComponent::Terminate()
{

}

void PipeComponent::Update(float deltaTime)
{
	mPipeXPos -= mPipeSpeed * deltaTime;
	if (mPipeXPos <= -mPipeWidth)
	{
		mPipeXPos += 263.0f * 3;
		Reset();
	}
}

void PipeComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Pipe Component##PipeComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat("X Pos##PipeComponent", &mPipeXPos, 1.0f, -500.0f, 500.0f);
		ImGui::DragFloat("Top Y Pos##PipeComponent", &mPipeYPos.x, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Bottom Y Pos##PipeComponent", &mPipeYPos.y, 1.0f, -1000.0f, 1000.0f);
	}
}

bool PipeComponent::CheckForBirdCollision(std::vector<SAGE::Math::Vector2> birdVerts)
{
	for (const Vector2& vert : birdVerts)
	{
		// Check top pipe
		if (vert.x >= mPipeXPos && vert.x <= mPipeXPos + mPipeWidth 
			&& vert.y <= mPipeYPos.x)
		{
			return true;
		}

		// Check bottom pipe
		if (vert.x >= mPipeXPos && vert.x <= mPipeXPos + mPipeWidth &&
			vert.y >= mPipeYPos.y && vert.y <= mPipeYPos.y + mPipeHeight)
		{
			return true;
		}
	}

	return false;;
}

void PipeComponent::Reset()
{
	const float randYPos = Random::UniformFloat(176.0f, 565.0f); // Gap of 96
	mPipeYPos.x = randYPos - 96.0f;
	mPipeYPos.y = randYPos + 96.0f;
}
