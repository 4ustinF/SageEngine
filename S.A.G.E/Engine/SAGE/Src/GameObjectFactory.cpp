#include "Precompiled.h"
#include "GameObjectFactory.h"

#include "GameObject.h"

#include "AnimatorComponent.h"
#include "BPhysicsComponent.h"
#include "CameraComponent.h"
#include "ColliderComponent.h"
#include "FollowCameraControllerComponent.h"
#include "FPSCameraControllerComponent.h"
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

	auto components = document["Components"].GetObj();
	for (auto& component : components)
	{
		const char* componentName = component.name.GetString();
		if (OnMake(componentName, component.value, gameObject)) {
			continue;
		}

		if (strcmp(componentName, "AnimatorComponent") == 0)
		{
			auto animator = gameObject.AddComponent<AnimatorComponent>();

			if (component.value.HasMember("AnimSets"))
			{
				const auto& animSets = component.value["AnimSets"].GetArray();

				for (auto& animset : animSets) {
					animator->AddAnimation(animset.GetString());
				}
			}
		}
		else if (strcmp(componentName, "CameraComponent") == 0)
		{
			auto cameraComponent = gameObject.AddComponent<CameraComponent>();
			auto& camera = cameraComponent->GetCamera();

			if (component.value.HasMember("Position"))
			{
				const auto& position = component.value["Position"].GetArray();
				const float x = position[0].GetFloat();
				const float y = position[1].GetFloat();
				const float z = position[2].GetFloat();
				camera.SetPosition({ x, y, z });
			}
			if (component.value.HasMember("Direction"))
			{
				const auto& direction = component.value["Direction"].GetArray();
				const float x = direction[0].GetFloat();
				const float y = direction[1].GetFloat();
				const float z = direction[2].GetFloat();
				camera.SetDirection({ x, y, z });
			}
			if (component.value.HasMember("LookAt"))
			{
				const auto& lookAt = component.value["LookAt"].GetArray();
				const float x = lookAt[0].GetFloat();
				const float y = lookAt[1].GetFloat();
				const float z = lookAt[2].GetFloat();
				camera.SetLookAt({ x, y, z });
			}
			//if (component.value.HasMember("Mode"))
			//{
			//	//camera.SetMode();
			//	//if (perspective)
			//	//{
			//	//	if (component.value.HasMember("FOV"))
			//	//	{

			//	//	}
			//	//	if (component.value.HasMember("AspectRatio"))
			//	//	{

			//	//	}
			//	//}
			//}
			if (component.value.HasMember("NearPlane"))
			{
				const auto nearPlane = component.value["NearPlane"].GetFloat();
				camera.SetNearPlane(nearPlane);
			}
			if (component.value.HasMember("FarPlane"))
			{
				const auto farPlane = component.value["FarPlane"].GetFloat();
				camera.SetFarPlane(farPlane);
			}
			if (component.value.HasMember("Zoom"))
			{
				const auto zoom = component.value["Zoom"].GetFloat();
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
			if (component.value.HasMember("Center"))
			{
				const auto& center = component.value["Center"].GetArray();
				const float x = center[0].GetFloat();
				const float y = center[1].GetFloat();
				const float z = center[2].GetFloat();
				colliderComponent->SetCenter({ x, y, z });
			}

			if (component.value.HasMember("Extend"))
			{
				const auto& extend = component.value["Extend"].GetArray();
				const float x = extend[0].GetFloat();
				const float y = extend[1].GetFloat();
				const float z = extend[2].GetFloat();
				colliderComponent->SetExtend({ x, y, z });
			}

			if (component.value.HasMember("Shape"))
			{
				const auto& shape = component.value["Shape"].GetString();
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
			if (component.value.HasMember("TargetName")) {
				const auto name = component.value["TargetName"].GetString();
				cameraComponent->SetTargetName(name);
			}
			if (component.value.HasMember("Distance")) {
				const auto distance = component.value["Distance"].GetFloat();
				cameraComponent->SetDistance(distance);
			}
			if (component.value.HasMember("CameraElevation")) {
				const auto elevation = component.value["CameraElevation"].GetFloat();
				cameraComponent->SetCameraElevation(elevation);
			}
			if (component.value.HasMember("LookElevation")) {
				const auto elevation = component.value["LookElevation"].GetFloat();
				cameraComponent->SetLookElevation(elevation);
			}
		}
		else if (strcmp(componentName, "FPSCameraControllerComponent") == 0)
		{
			auto cameraComponent = gameObject.AddComponent<FPSCameraControllerComponent>();
			if (component.value.HasMember("NormSpeed")) {
				const auto speed = component.value["NormSpeed"].GetFloat();
				cameraComponent->SetNormSpeed(speed);
			}

			if (component.value.HasMember("FastSpeed")) {
				const auto speed = component.value["FastSpeed"].GetFloat();
				cameraComponent->SetFastSpeed(speed);
			}

			if (component.value.HasMember("TurnSpeed")) {
				const auto speed = component.value["TurnSpeed"].GetFloat();
				cameraComponent->SetTurnSpeed(speed);
			}
		}
		else if (strcmp(componentName, "ModelComponent") == 0)
		{
			auto modelComponent = gameObject.AddComponent<ModelComponent>();
			if (component.value.HasMember("FileName"))
			{
				const char* fileName = component.value["FileName"].GetString();
				modelComponent->SetFileName(fileName);
			}
			if (component.value.HasMember("Rotation"))
			{
				const auto& rotation = component.value["Rotation"].GetArray();
				const float x = rotation[0].GetFloat() * Math::Constants::DegToRad;
				const float y = rotation[1].GetFloat() * Math::Constants::DegToRad;
				const float z = rotation[2].GetFloat() * Math::Constants::DegToRad;
				modelComponent->SetRotation({ x, y, z });
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

			if (component.value.HasMember("Mass"))
			{
				const float mass = component.value["Mass"].GetFloat();
				rigidBodyComponent->SetMass(mass);
			}

			if (component.value.HasMember("Constraints"))
			{
				const auto contraints = component.value["Constraints"].GetArray();

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
			if (component.value.HasMember("FileName"))
			{
				const char* fileName = component.value["FileName"].GetString();
				spawnerComponent->SetFileName(fileName);
			}
			if (component.value.HasMember("MaxSpawnCount"))
			{
				const auto count = component.value["MaxSpawnCount"].GetInt();
				spawnerComponent->SetMaxSpawnCount(count);
			}
			if (component.value.HasMember("WaitTime"))
			{
				const auto waitTime = component.value["WaitTime"].GetFloat();
				spawnerComponent->SetWaitTime(waitTime);
			}
		}
		else if (strcmp(componentName, "TransformComponent") == 0)
		{
			auto transformComponent = gameObject.AddComponent<TransformComponent>();
			if (component.value.HasMember("Position"))
			{
				const auto& position = component.value["Position"].GetArray();
				const float x = position[0].GetFloat();
				const float y = position[1].GetFloat();
				const float z = position[2].GetFloat();
				transformComponent->position = { x, y, z };
			}

			if (component.value.HasMember("Rotation"))
			{
				const auto& rotation = component.value["Rotation"].GetArray();
				const float x = rotation[0].GetFloat() * Math::Constants::DegToRad;
				const float y = rotation[1].GetFloat() * Math::Constants::DegToRad;
				const float z = rotation[2].GetFloat() * Math::Constants::DegToRad;
				transformComponent->rotation = Math::Quaternion::RotationEuler({ x, y, z });
			}

			if (component.value.HasMember("Scale"))
			{
				const auto& scale = component.value["Scale"].GetArray();
				const float x = scale[0].GetFloat();
				const float y = scale[1].GetFloat();
				const float z = scale[2].GetFloat();
				transformComponent->scale = { x, y, z };
			}
		}
		// ... more components here
	}
}