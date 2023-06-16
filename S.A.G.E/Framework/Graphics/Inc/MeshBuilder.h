#pragma once

#include "MeshTypes.h"

namespace SAGE::Graphics
{
	class MeshBuilder
	{
	public:
		static MeshPC CreateCubePC();
		static MeshPC CreatePlanePC(int columns, int rows);
		static MeshPC CreateCylinderPC(int slices, int rings);
		static MeshPC CreateSpherePC(int slices, int rings, float radius);

		static MeshPX CreateCubePX();
		static MeshPX CreateSkyBoxPX();
		static MeshPX CreateSpherePX(int slices, int rings, float radius);
		static MeshPX CreateSkydomePX(int slices, int rings, float radius);
		static MeshPX CreateScreenQuad();

		static Mesh CreateCube();
		static Mesh CreateSkyBox();
		static Mesh CreateCylinder(const Math::Cylinder& cylinder);
		static Mesh CreateCylinder(int slices, int rings);
		static Mesh CreateSphere(const Math::Sphere& sphere);
		static Mesh CreateSphere(int slices, int rings, float radius);
		static Mesh CreateSkyDome(const Math::Sphere& sphere);
		static Mesh CreateSkyDome(int slices, int rings, float radius);
		static Mesh CreatePlane(int columns, int rows, float spacing, bool flipVertices = false);
	};
}