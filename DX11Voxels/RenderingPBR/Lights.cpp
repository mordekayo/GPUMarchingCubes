#include "Lights.h"

#include "../GameFramework/VertexPositionColor.h"
#include "../GameFramework/FbxLoaderExt.h"
#include "../GameFramework/DebugRenderSystem.h"
#include "../GameFramework/StaticMesh.h"


#pragma region SpotLight
StaticMesh* SpotLight::Spot = nullptr;

void SpotLight::CheckAngles()
{
	if (phiPenumbra < thetaUmbra) phiPenumbra = thetaUmbra;

	float total = thetaUmbra + phiPenumbra;
	if (total >= M_PI)
	{
		thetaUmbra = (thetaUmbra / total) * (M_PI - 0.001);
		phiPenumbra = (phiPenumbra / total) * (M_PI - 0.001);
	}
}

void SpotLight::SetAngles(float inThetaUmbra, float inPhiPenumbra)
{
	thetaUmbra = inThetaUmbra;
	phiPenumbra = inPhiPenumbra;
	CheckAngles();
}

SpotLight::SpotLight(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 direction, float length,
						float umbra, float penumbra, float falloff, float intensity,
						DirectX::SimpleMath::Color color) : thetaUmbra(0), phiPenumbra(0), LightBase()
{
	LightType = LightTypes::Spot;

	PosDir = position;

	SetAngles(umbra, penumbra);
	Falloff = falloff;
	Length = length;
	LightColor = color;
	Intensity = intensity;

	direction.Normalize(Direction);

	if (Spot == nullptr) Spot = LoadFbxStaticMesh<VertexPositionColor>("./Content/Lights/spot0.fbx");

	MeshRep = Spot;
}

Matrix SpotLight::CalcTransformMatrix() const
{
	Vector3 forward;
	Direction.Normalize(forward);
	//TODO : Check correctness of this func
	Vector3 right = Vector3::Cross(forward, Vector3::Up);
	if (right.Length() < 0.0001f)
	{
		right = Vector3::Cross(forward, Vector3::Backward);
	}
	right.Normalize();

	Vector3 up = Vector3::Cross(forward, right);

	auto m = Matrix::Identity;

	m.Forward(right);
	m.Right(forward);
	m.Up(up);

	auto tanPhi = (float)std::tan(phiPenumbra);
	auto ret = Matrix::CreateScale(1, Length * tanPhi, Length * tanPhi) * Matrix::CreateScale(Length, 1, 1) * m * Matrix
		::CreateTranslation(PosDir);

	return ret;
}

void SpotLight::DrawDebugData()
{
	if (isDrawDebug) Game::Instance->DebugRender->DrawStaticMesh(*MeshRep, CalcTransformMatrix(),
	                                                             Color(1.0f, 0.0f, 0.0f, 1.0f));
}

LightData SpotLight::GetLightData()
{
	return LightData{
		Vector4(PosDir.x, PosDir.y, PosDir.z, 1.0f),
		Vector4(Direction.x, Direction.y, Direction.z, Intensity),
		Vector4(Length, thetaUmbra, phiPenumbra, Falloff),
		LightColor
	};
}

PixelFlagsLighting SpotLight::GetLightFlags()
{
	return PixelFlagsLighting::SpotLight;
}

Matrix SpotLight::GetTransformMatrix()
{
	return CalcTransformMatrix();
}

void SpotLight::Update(float dt)
{
}

#pragma endregion SpotLight


#pragma region PointLight

StaticMesh* PointLight::Sphere = nullptr;


PointLight::PointLight(DirectX::SimpleMath::Vector3 position, float radius, float attenRadius, float intensity,
                       DirectX::SimpleMath::Color color)
{
	LightType = LightTypes::Point;

	PosDir = position;
	LightColor = color;
	Intensity = intensity;
	Radius = radius < attenRadius ? radius : attenRadius;
	AttenRadius = radius < attenRadius ? attenRadius : radius;

	if (Sphere == nullptr) Sphere = LoadFbxStaticMesh<VertexPositionColor>("./Content/Lights/point0.fbx");

	MeshRep = Sphere;
}


void PointLight::DrawDebugData()
{
	if (isDrawDebug)
	{
		Game::Instance->DebugRender->DrawSphere(Radius, Color(0.4f, 0.4f, 0.4f, 1.0f), Matrix::CreateTranslation(PosDir));
		//Game.Instance.DebugRender.DrawSphere(AttenRadius, SharpDX.Color.YellowGreen, Matrix.Translation(PosDir));
	}
}


LightData PointLight::GetLightData()
{
	return LightData
	{
		Vector4(PosDir.x, PosDir.y, PosDir.z, 1.0f),
		Vector4(PosDir.x, PosDir.y, PosDir.z, 1.0f),
		Vector4(Radius, AttenRadius, Intensity, 0),
		LightColor,
	};
}


void PointLight::Update(float dt)
{
}


Matrix PointLight::GetTransformMatrix()
{
	float scale = 1.0f;
	return Matrix::CreateScale(Radius * scale, Radius * scale, Radius * scale) * Matrix::CreateTranslation(PosDir);
}


PixelFlagsLighting PointLight::GetLightFlags()
{
	return PixelFlagsLighting::PointLight;
}

#pragma endregion PointLight


#pragma region AmbientLight

AmbientLight::AmbientLight(float intensity, float specIntensity, Color color) : SpecIntensity(specIntensity)
{
	LightType = LightTypes::Ambient;
	Intensity = intensity;
	LightColor = color;
}


void AmbientLight::DrawDebugData()
{
	// how?
}

LightData AmbientLight::GetLightData()
{
	return LightData
	{
		Vector4(0, 0, 0, 1.0f),
		Vector4(0, 0, 0, 1.0f),
		Vector4(Intensity, SpecIntensity, 0, 0),
		LightColor
	};
}

PixelFlagsLighting AmbientLight::GetLightFlags()
{
	return PixelFlagsLighting::AmbientLight;
}

Matrix AmbientLight::GetTransformMatrix()
{
	return Matrix::Identity;
}

void AmbientLight::Update(float dt)
{
}

#pragma endregion AmbientLight


#pragma region DirectionalLight

DirectionalLight::DirectionalLight(Vector3 direction, float intensity, Color color)
{
	LightType = LightTypes::Directional;

	PosDir = direction;
	LightColor = color;
	Intensity = intensity;

	PosDir.Normalize();
}

void DirectionalLight::DrawDebugData()
{
	if (isDrawDebug) Game::Instance->DebugRender->DrawArrow(Vector3::Zero, Vector3::Zero + PosDir * 10,
	                                                        Color(1.0f, 0.0f, 0.0f, 1.0f), Vector3::Up);
}

LightData DirectionalLight::GetLightData()
{
	return LightData
	{
		Vector4(PosDir.x, PosDir.y, PosDir.z, 0.0f),
		Vector4(PosDir.x, PosDir.y, PosDir.z, 0.0f),
		Vector4(Intensity, 0, 0, 0),
		LightColor
	};
}

PixelFlagsLighting DirectionalLight::GetLightFlags()
{
	return PixelFlagsLighting::DirectionalLight;
}

Matrix DirectionalLight::GetTransformMatrix()
{
	return Matrix::Identity;
}

void DirectionalLight::Update(float dt)
{
}

#pragma endregion DirectionalLight

