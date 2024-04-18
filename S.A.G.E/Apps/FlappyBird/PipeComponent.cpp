#include "PipeComponent.h"

using namespace SAGE;
using namespace SAGE::Input;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

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


void PipeComponent::Reset()
{
	const float randYPos = Random::UniformFloat(176.0f, 565.0f); // Gap of 96
	mPipeYPos.x = randYPos - 96.0f;
	mPipeYPos.y = randYPos + 96.0f;
}