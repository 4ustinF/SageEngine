#include "Precompiled.h"
#include "GameObjectFactory.h"

#include "GameObject.h"

#include "AnimatorComponent.h"
#include "BPhysicsComponent.h"
#include "CameraComponent.h"
#include "ColliderComponent.h"
#include "FollowCameraControllerComponent.h"
#include "FPSCameraControllerComponent.h"
#include "MeshFilterComponent.h"
#include "MeshRendererComponent.h"
#include "ModelComponent.h"
#include "ParticleComponent.h"
#include "RigidBodyComponent.h"
#include "SpawnerComponent.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
namespace rj = rapidjson;

namespace
{
	MakeOverride OnMake;
}

void GameObjectFactory::SetMakeOverride(MakeOverride makeOverride)
{
	OnMake = makeOverride;
}

void GameObjectFactory::Make(std::filesystem::path templateFile, GameObject& gameObject)
{
	FILE* file = nullptr;
	auto err = fopen_s(&file, templateFile.u8string().c_str(), "r");
	ASSERT(err == 0 && file != nullptr, "GameObjectFactory --- Failed to open template file '%s'", templateFile.u8string().c_str());

	char readBuffer[65536];
	rj::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));

	fclose(file);

	rj::Document document;
	document.ParseStream(readStream);

	if (document.HasMember("ObjectName") && document["ObjectName"].IsString())
	{
		gameObject.SetName(document["ObjectName"].GetString());
	}

	if (document.HasMember("HierarchyPath") && document["HierarchyPath"].IsString())
	{
		gameObject.SetHierarchyPath(document["HierarchyPath"].GetString());
	}

	auto components = document["Components"].GetObj();
	for (auto& component : components)
	{
		const char* componentName = component.name.GetString();
		TryMakeComponent(componentName, component.value, gameObject);
	}
}

void GameObjectFactory::TryMakeComponent(const char* componentName, const rapidjson::Value& value, GameObject& gameObject)
{
	if (OnMake(componentName, value, gameObject)) {
		return;
	}

	if (strcmp(componentName, "AnimatorComponent") == 0)
	{
		auto animator = gameObject.AddComponent<AnimatorComponent>();

		if (value.HasMember("AnimSets"))
		{
			const auto& animSets = value["AnimSets"].GetArray();

			for (auto& animset : animSets) {
				animator->AddAnimation(animset.GetString());
			}
		}
	}
	else if (strcmp(componentName, "CameraComponent") == 0)
	{
		auto cameraComponent = gameObject.AddComponent<CameraComponent>();
		auto& camera = cameraComponent->GetCamera();

		if (value.HasMember("Position"))
		{
			const auto& position = value["Position"].GetArray();
			const float x = position[0].GetFloat();
			const float y = position[1].GetFloat();
			const float z = position[2].GetFloat();
			camera.SetPosition({ x, y, z });
		}
		if (value.HasMember("Direction"))
		{
			const auto& direction = value["Direction"].GetArray();
			const float x = direction[0].GetFloat();
			const float y = direction[1].GetFloat();
			const float z = direction[2].GetFloat();
			camera.SetDirection({ x, y, z });
		}
		if (value.HasMember("LookAt"))
		{
			const auto& lookAt = value["LookAt"].GetArray();
			const float x = lookAt[0].GetFloat();
			const float y = lookAt[1].GetFloat();
			const float z = lookAt[2].GetFloat();
			camera.SetLookAt({ x, y, z });
		}
		//if (value.HasMember("Mode"))
		//{
		//	//camera.SetMode();
		//	//if (perspective)
		//	//{
		//	//	if (value.HasMember("FOV"))
		//	//	{

		//	//	}
		//	//	if (value.HasMember("AspectRatio"))
		//	//	{

		//	//	}
		//	//}
		//}
		if (value.HasMember("NearPlane"))
		{
			const auto nearPlane = value["NearPlane"].GetFloat();
			camera.SetNearPlane(nearPlane);
		}
		if (value.HasMember("FarPlane"))
		{
			const auto farPlane = value["FarPlane"].GetFloat();
			camera.SetFarPlane(farPlane);
		}
		if (value.HasMember("Zoom"))
		{
			const auto zoom = value["Zoom"].GetFloat();
			camera.Zoom(zoom);
		}
		//
		// TODO
		// Set camera pos, dir, lookat
		// mode
		// if perspective fov, aspect raito
		// if ortohographic, read size
		// near / far plane
		// zoom
	}
	else if (strcmp(componentName, "ColliderComponent") == 0)
	{
		auto colliderComponent = gameObject.AddComponent<ColliderComponent>();
		if (value.HasMember("Center"))
		{
			const auto& center = value["Center"].GetArray();
			const float x = center[0].GetFloat();
			const float y = center[1].GetFloat();
			const float z = center[2].GetFloat();
			colliderComponent->SetCenter({ x, y, z });
		}

		if (value.HasMember("Extend"))
		{
			const auto& extend = value["Extend"].GetArray();
			const float x = extend[0].GetFloat();
			const float y = extend[1].GetFloat();
			const float z = extend[2].GetFloat();
			colliderComponent->SetExtend({ x, y, z });
		}

		if (value.HasMember("Shape"))
		{
			const auto& shape = value["Shape"].GetString();
			if (strcmp(shape, "Sphere") == 0)
			{
				colliderComponent->SetColliderShape(ColliderShapes::Sphere);
			}
			else if (strcmp(shape, "Capsule") == 0)
			{
				colliderComponent->SetColliderShape(ColliderShapes::Capsule);
			}
			else if (strcmp(shape, "StaticPlane") == 0)
			{
				colliderComponent->SetColliderShape(ColliderShapes::StaticPlane);
			}
			else
			{
				colliderComponent->SetColliderShape(ColliderShapes::Box);
			}
		}
	}
	else if (strcmp(componentName, "FollowCameraControllerComponent") == 0)
	{
		auto cameraComponent = gameObject.AddComponent<FollowCameraControllerComponent>();
		if (value.HasMember("TargetName")) {
			const auto name = value["TargetName"].GetString();
			cameraComponent->SetTargetName(name);
		}
		if (value.HasMember("Distance")) {
			const auto distance = value["Distance"].GetFloat();
			cameraComponent->SetDistance(distance);
		}
		if (value.HasMember("CameraElevation")) {
			const auto elevation = value["CameraElevation"].GetFloat();
			cameraComponent->SetCameraElevation(elevation);
		}
		if (value.HasMember("LookElevation")) {
			const auto elevation = value["LookElevation"].GetFloat();
			cameraComponent->SetLookElevation(elevation);
		}
	}
	else if (strcmp(componentName, "FPSCameraControllerComponent") == 0)
	{
		auto cameraComponent = gameObject.AddComponent<FPSCameraControllerComponent>();
		if (value.HasMember("NormSpeed")) {
			const auto speed = value["NormSpeed"].GetFloat();
			cameraComponent->SetNormSpeed(speed);
		}

		if (value.HasMember("FastSpeed")) {
			const auto speed = value["FastSpeed"].GetFloat();
			cameraComponent->SetFastSpeed(speed);
		}

		if (value.HasMember("TurnSpeed")) {
			const auto speed = value["TurnSpeed"].GetFloat();
			cameraComponent->SetTurnSpeed(speed);
		}
	}
	else if (strcmp(componentName, "Mesh Filter Component") == 0)
	{
		auto meshFilterComponent = gameObject.AddComponent<MeshFilterComponent>();
	}
	else if (strcmp(componentName, "Mesh Renderer Component") == 0)
	{
		MeshRendererComponent* meshRendererComponent = gameObject.AddComponent<MeshRendererComponent>();

		// Mesh Renderer Requires a Mesh Filter Component as well.
		MeshFilterComponent* meshFilterComponent = gameObject.GetComponent<MeshFilterComponent>();
		if (meshFilterComponent == nullptr)
		{
			meshFilterComponent = gameObject.AddComponent<MeshFilterComponent>();
		}

		meshRendererComponent->SetMeshFilterComponent(meshFilterComponent);
	}
	else if (strcmp(componentName, "Model Component") == 0)
	{
		auto modelComponent = gameObject.AddComponent<ModelComponent>();
		if (value.HasMember("FileName"))
		{
			const char* fileName = value["FileName"].GetString();
			modelComponent->SetFileName(fileName);
		}
		if (value.HasMember("Rotation"))
		{
			const auto& rotation = value["Rotation"].GetArray();
			const float x = rotation[0].GetFloat() * Math::Constants::DegToRad;
			const float y = rotation[1].GetFloat() * Math::Constants::DegToRad;
			const float z = rotation[2].GetFloat() * Math::Constants::DegToRad;
			modelComponent->SetRotation({ x, y, z });
		}
		if (value.HasMember("IsBasicModel"))
		{
			const bool isBasic = value["IsBasicModel"].GetBool();
			modelComponent->SetIsBasicModel(isBasic);
		}
	}
	else if (strcmp(componentName, "ParticleComponent") == 0)
	{
		auto modelComponent = gameObject.AddComponent<ParticleComponent>();
	}
	else if (strcmp(componentName, "RigidBodyComponent") == 0)
	{
		auto rigidBodyComponent = gameObject.AddComponent<RigidBodyComponent>();
		auto physicsComponent = gameObject.AddComponent<BPhysicsComponent>();

		if (value.HasMember("Mass"))
		{
			const float mass = value["Mass"].GetFloat();
			rigidBodyComponent->SetMass(mass);
		}

		if (value.HasMember("Constraints"))
		{
			const auto contraints = value["Constraints"].GetArray();

			const int posX = contraints[0].GetInt();
			const int posY = contraints[1].GetInt();
			const int posZ = contraints[2].GetInt();
			const int rotX = contraints[3].GetInt();
			const int rotY = contraints[4].GetInt();
			const int rotZ = contraints[5].GetInt();

			rigidBodyComponent->SetConstraints(Vector3Int(posX, posY, posZ), Vector3Int(rotX, rotY, rotZ));
		}
	}
	else if (strcmp(componentName, "SpawnerComponent") == 0)
	{
		auto spawnerComponent = gameObject.AddComponent<SpawnerComponent>();
		if (value.HasMember("FileName"))
		{
			const char* fileName = value["FileName"].GetString();
			spawnerComponent->SetFileName(fileName);
		}
		if (value.HasMember("MaxSpawnCount"))
		{
			const auto count = value["MaxSpawnCount"].GetInt();
			spawnerComponent->SetMaxSpawnCount(count);
		}
		if (value.HasMember("WaitTime"))
		{
			const auto waitTime = value["WaitTime"].GetFloat();
			spawnerComponent->SetWaitTime(waitTime);
		}
	}
	else if (strcmp(componentName, "Transform Component") == 0)
	{
		auto transformComponent = gameObject.AddComponent<TransformComponent>();
		if (value.HasMember("Position"))
		{
			const auto& position = value["Position"].GetArray();
			const float x = position[0].GetFloat();
			const float y = position[1].GetFloat();
			const float z = position[2].GetFloat();
			transformComponent->SetPosition(Vector3(x, y, z));
		}

		if (value.HasMember("Rotation"))
		{
			const auto& rotation = value["Rotation"].GetArray();
			const float x = rotation[0].GetFloat();
			const float y = rotation[1].GetFloat();
			const float z = rotation[2].GetFloat();
			transformComponent->SetRotation(Vector3(x, y, z));
		}

		if (value.HasMember("Scale"))
		{
			const auto& scale = value["Scale"].GetArray();
			const float x = scale[0].GetFloat();
			const float y = scale[1].GetFloat();
			const float z = scale[2].GetFloat();
			transformComponent->SetScale(Vector3(x, y, z));
		}
	}
	// ... more components here
}