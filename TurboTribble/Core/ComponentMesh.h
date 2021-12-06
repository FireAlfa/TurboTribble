#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "Component.h"

#include "Globals.h"

#include <string.h>
#include "Math/float3.h"
#include "Math/float2.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include "Geometry/Frustum.h"
#include "ParShapes/par_shapes.h"



class ComponentMesh : public Component
{

public:

	enum class Shape
	{
		CUBE,
		SPHERE,
		CYLINDER
	};

	ComponentMesh(GameObject* parent);
	ComponentMesh(GameObject* parent, Shape shape);
	~ComponentMesh();

	void CopyParMesh(par_shapes_mesh* parMesh);

	void GenerateBuffers();
	void ComputeNormals();
	void GenerateBounds();
	void DrawNormals() const;
	float3 GetCenterPointInWorldCoords() const;
	inline float GetSphereRadius() const { return radius; }

	bool Update(float dt) override;
	void OnGui() override;

	void DrawDebug() override;

private:

	void Render();
	bool CameraCulling(Frustum* cameraFrustum);

public:

	uint vertexBufferId = 0, indexBufferId = 0, textureBufferId = 0;
	std::string texturePath;

	uint numVertices = 0;
	std::vector<float3> vertices;

	uint numNormalFaces = 0;
	std::vector<float3> normals;
	std::vector<float3> faceNormals;
	std::vector<float3> faceCenters;

	std::vector<float2> texCoords;

	uint numIndices = 0;
	std::vector<uint> indices;

	bool drawWireframe = false;
	bool drawVertexNormals = false;
	bool drawFaceNormals = false;
	float normalScale = 1.f;
	bool drawable = true;

	// Bounding Box
	AABB localAABB;
	OBB globalOBB;
	AABB globalAABB;

private:

	// Bounding Sphere
	float3 centerPoint = float3::zero;
	float radius;

};

#endif // !__COMPONENT_MESH_H__