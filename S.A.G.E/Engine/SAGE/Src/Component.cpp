#include "Precompiled.h"
#include "Component.h"

#include <commdlg.h>

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

std::string Component::OpenFileDialog(const char* fileFilterType)
{
	char fileName[MAX_PATH] = "";

	OPENFILENAMEA ofn = {};
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;

	ofn.lpstrFilter = fileFilterType;

	ofn.Flags =
		OFN_FILEMUSTEXIST |
		OFN_PATHMUSTEXIST |
		OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn))
	{
		return fileName;
	}

	return "";
}

void Component::SaveBoolToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, bool value)
{
	if (title.empty())
	{
		return;
	}

	compObj.AddMember(rj::Value(title.c_str(), allocator), rj::Value(value), allocator);
}

void Component::SaveStringToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const std::string& value)
{
	if (title.empty() || value.empty())
	{
		return;
	}

	compObj.AddMember(rj::Value(title.c_str(), allocator), rj::Value(value.c_str(), allocator), allocator);
}

void Component::SaveNumberToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, int value, int defaultValue)
{
	if (title.empty() || value == defaultValue)
	{
		return;
	}

	rj::Value numValue(rj::kNumberType);
	numValue.SetInt(value);
	compObj.AddMember(rj::Value(title.c_str(), allocator), numValue, allocator);
}

void Component::SaveNumberToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, float value, float defaultValue)
{
	if (title.empty() || value == defaultValue)
	{
		return;
	}

	rj::Value numValue(rj::kNumberType);
	numValue.SetFloat(value);
	compObj.AddMember(rj::Value(title.c_str(), allocator), numValue, allocator);
}

void Component::SaveColorToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const Color& value, const Color& defaultValue)
{
	SaveVector4ToTemplate(compObj, allocator, title, value, defaultValue);
}

void Component::SaveVector2ToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const Vector2& value, const Vector2& defaultValue)
{
	if (title.empty() || value == defaultValue)
	{
		return;
	}

	rj::Value vec2(rj::kArrayType);
	vec2.PushBack(value.x, allocator);
	vec2.PushBack(value.y, allocator);
	compObj.AddMember(rj::Value(title.c_str(), allocator), vec2, allocator);
}

void Component::SaveVector2IntToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const SAGE::Math::Vector2Int& value, const SAGE::Math::Vector2Int& defaultValue)
{
	if (title.empty() || value == defaultValue)
	{
		return;
	}

	rj::Value vec2(rj::kArrayType);
	vec2.PushBack(value.x, allocator);
	vec2.PushBack(value.y, allocator);
	compObj.AddMember(rj::Value(title.c_str(), allocator), vec2, allocator);
}

void Component::SaveVector3ToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const Vector3& value, const Vector3& defaultValue)
{
	if (title.empty() || value == defaultValue)
	{
		return;
	}

	rj::Value vec3(rj::kArrayType);
	vec3.PushBack(value.x, allocator);
	vec3.PushBack(value.y, allocator);
	vec3.PushBack(value.z, allocator);
	compObj.AddMember(rj::Value(title.c_str(), allocator), vec3, allocator);
}

void Component::SaveVector4ToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator, const std::string& title, const Vector4& value, const Vector4& defaultValue)
{
	if (title.empty() || value == defaultValue)
	{
		return;
	}

	rj::Value vec4(rj::kArrayType);
	vec4.PushBack(value.x, allocator);
	vec4.PushBack(value.y, allocator);
	vec4.PushBack(value.z, allocator);
	vec4.PushBack(value.w, allocator);
	compObj.AddMember(rj::Value(title.c_str(), allocator), vec4, allocator);
}