#pragma once

#include <SAGE/Inc/SAGE.h>
#include "SAGE/Inc/BPhysicsService.h"

class ChunkRenderer
{
public:
	ChunkRenderer(SAGE::BPhysicsService* bPhysicsService)
	{
		auto tm = SAGE::Graphics::TextureManager::Get();
		opaqueChunkRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
		opaqueChunkRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
		opaqueChunkRenderObject.material.specular = { 0.2f, 0.2f, 0.2f, 1.0f };
		opaqueChunkRenderObject.material.emissive = { 0.5f, 0.5f, 0.5f, 1.0f };
		opaqueChunkRenderObject.material.power = 10.0f;

		transparentChunkRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
		transparentChunkRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
		transparentChunkRenderObject.material.specular = { 0.2f, 0.2f, 0.2f, 1.0f };
		transparentChunkRenderObject.material.emissive = { 0.5f, 0.5f, 0.5f, 1.0f };
		transparentChunkRenderObject.material.power = 10.0f;

		waterChunkRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 0.3f };
		waterChunkRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
		waterChunkRenderObject.material.specular = { 0.1f, 0.1f, 0.1f, 1.0f };
		waterChunkRenderObject.material.emissive = { 0.5f, 0.5f, 0.5f, 0.4f };
		waterChunkRenderObject.material.power = 10.0f;
		waterChunkRenderObject.diffuseMapId = tm->LoadTexture("../Sprites/SageCraft/Water.png");
		waterChunkRenderObject.normalMapId = tm->LoadTexture("../Sprites/SageCraft/WaterNormal.png");
		waterChunkRenderObject.specularMapId = tm->LoadTexture("../Sprites/SageCraft/WaterSpecular.png");

		BPhysicsService = bPhysicsService;
	}

	~ChunkRenderer()
	{
		opaqueChunkRenderObject.Terminate();
		transparentChunkRenderObject.Terminate();
		waterChunkRenderObject.Terminate();
		activeBlocks.clear();
		activeBlocksIndexs.clear();
		vertices.clear();

		//RemoveChunkCollider();
		BPhysicsService = nullptr;
	}

	void RemoveChunkCollider()
	{
		if (chunkRB != nullptr)
		{
			BPhysicsService->GetDynamicsWorld()->removeRigidBody(chunkRB);
		}

		if (chunkRB != nullptr && chunkRB->getMotionState())
		{
			delete chunkRB->getMotionState();
		}
		SafeDelete(chunkRB);
	}

	void CreateChunkCollider()
	{
		if (vertices.empty())
		{
			return;
		}

		// Create a static mesh shape for a static object
		btTriangleMesh* triangleMesh = new btTriangleMesh();

		// Add the triangles to the triangle mesh
		for (int i = 0; i < vertices.size(); i += 3) {
			const btVector3 vertex1 = ConvertToBtvector3(vertices[i]) * btVector3(1.0f, -1.0f, -1.0f);
			const btVector3 vertex2 = ConvertToBtvector3(vertices[i + 1]) * btVector3(1.0f, -1.0f, -1.0f);
			const btVector3 vertex3 = ConvertToBtvector3(vertices[i + 2]) * btVector3(1.0f, -1.0f, -1.0f);
			triangleMesh->addTriangle(vertex1, vertex2, vertex3, true);
		}

		// Create a btBvhTriangleMeshShape object from the triangle mesh
		btBvhTriangleMeshShape* staticShape = new btBvhTriangleMeshShape(triangleMesh, true);
		btVector3 localIntertia(0.0f, 0.0f, 0.0f);

		btDefaultMotionState* motionState = new btDefaultMotionState(ConvertToBtTransform(SAGE::Graphics::Transform()));
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, staticShape, localIntertia);
		chunkRB = new btRigidBody(rbInfo);

		chunkRB->setUserIndex(SAGE::CollisionGroups::Default);
		BPhysicsService->GetDynamicsWorld()->addRigidBody(chunkRB, SAGE::CollisionGroups::Default, SAGE::CollisionGroups::Default);
		vertices.clear();
	}

	bool transChunkActive = false;
	bool waterChunkActive = false;

	SAGE::Graphics::RenderObject opaqueChunkRenderObject;
	SAGE::Graphics::RenderObject transparentChunkRenderObject;
	SAGE::Graphics::RenderObject waterChunkRenderObject;

	std::vector<SAGE::Math::Vector3> activeBlocks;
	std::vector<SAGE::Math::Vector3Int> activeBlocksIndexs;
	std::vector<SAGE::Math::Vector3> vertices;
	SAGE::BPhysicsService* BPhysicsService = nullptr;
	btRigidBody* chunkRB = nullptr;
};