#include "pch.h"
#include "MeshTriangle.h"
#include "Mesh.h"


Vector3 FbxNative::MeshTriangle::ComputeNormal(Mesh* mesh)
{
	Vector3 p0 = mesh->Vertices[Index0].Position;
	Vector3 p1 = mesh->Vertices[Index1].Position;
	Vector3 p2 = mesh->Vertices[Index2].Position;

	auto p1p0 = p1 - p0;
	auto p2p0 = p2 - p0;
	
	Vector3 n = Vector3::Cross(p1p0, p2p0);
	return	n;
}

bool FbxNative::MeshTriangle::IsDegenerate()
{
	bool isDegnerate = (Index0 == Index1 || Index0 == Index2 || Index1 == Index2);
	return isDegnerate;
}

Vector3 FbxNative::MeshTriangle::Centroid(Mesh* mesh)
{
	auto p0 = mesh->Vertices[Index0].Position;
	auto p1 = mesh->Vertices[Index1].Position;
	auto p2 = mesh->Vertices[Index2].Position;

	return (p0 + p1 + p2) / 3;
}
