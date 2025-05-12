#pragma once
#include <d3d.h>
#include <d3d11.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "../ZMathLib/SimpleMath.h"
#include "ShadersFlags.h"
#include "../GameFramework/VertexPositionColor.h"
#include "../GameFramework/FbxLoaderExt.h"



enum class LightTypes
{
	Ambient = 0,
	Directional = 1,
	Point = 2,
	Spot = 3,
};


#pragma pack(push, 4)
struct LightData {
	DirectX::SimpleMath::Vector4	Pos;
	DirectX::SimpleMath::Vector4	Dir;
	DirectX::SimpleMath::Vector4	Params;
	DirectX::SimpleMath::Color		Color;
};
#pragma pack(pop)


class LightBase
{
public:
	LightTypes LightType = LightTypes::Ambient;
	StaticMesh* MeshRep = nullptr;
	
	DirectX::SimpleMath::Vector3	PosDir;
	DirectX::SimpleMath::Color		LightColor;
	float	Intensity = 1.0f;

	bool isDrawDebug = false;

	virtual void Update(float dt)		= 0;
	virtual LightData GetLightData()	= 0;
	virtual DirectX::SimpleMath::Matrix GetTransformMatrix() = 0;
	virtual void DrawDebugData()		= 0;

	virtual PixelFlagsLighting GetLightFlags() = 0;

	LightBase() {};
	virtual ~LightBase() = default;
};


class SpotLight : public LightBase
{
protected:
	float thetaUmbra;
	float phiPenumbra;

public:
	static StaticMesh* Spot;

	float Falloff;
	float Length;
	DirectX::SimpleMath::Vector3 Direction;


	inline void		SetThetaUmbra(float thetaUmbraRad) { thetaUmbra = thetaUmbraRad; CheckAngles(); }
	inline float	GetThetaUmbra() const { return thetaUmbra; }// [0, pi)
	inline void		SetPhiPenumbra(float phiPenumbraRad) { phiPenumbra = phiPenumbraRad; CheckAngles(); } // [theta, pi)
	inline float	GetPhiPenumbra() const { return phiPenumbra; }


	void CheckAngles();
	void SetAngles(float inThetaUmbra, float inPhiPenumbra);


	SpotLight(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 direction, float length, float umbra,
	          float penumbra, float falloff, float intensity, DirectX::SimpleMath::Color color);

	[[nodiscard]] DirectX::SimpleMath::Matrix CalcTransformMatrix() const;

	virtual void				DrawDebugData()			override;
	virtual LightData			GetLightData()			override;
	virtual PixelFlagsLighting	GetLightFlags()			override;

	virtual DirectX::SimpleMath::Matrix	GetTransformMatrix()	override;

	virtual void Update(float dt) override;
};


class PointLight : public LightBase
{
public:
	float Radius;
	float AttenRadius;

	static StaticMesh* Sphere;

	PointLight(DirectX::SimpleMath::Vector3 position, float radius, float attenRadius, float intensity,
		DirectX::SimpleMath::Color color);

	virtual void Update(float dt)	override;
	virtual void DrawDebugData()	override;
	virtual Matrix		GetTransformMatrix()	override;
	virtual LightData	GetLightData()			override;
	virtual PixelFlagsLighting GetLightFlags()	override;
};


class AmbientLight : public LightBase
{
public:

	float SpecIntensity;

	AmbientLight(float intensity, float specIntensity, Color color);

	virtual void DrawDebugData() override;
	virtual LightData GetLightData() override;
	virtual PixelFlagsLighting GetLightFlags() override;
	virtual Matrix GetTransformMatrix() override;

	virtual void Update(float dt) override;
};


class DirectionalLight : public LightBase
{
public:
	float AmbientAmount;

	DirectionalLight(Vector3 direction, float intensity, Color color);

	virtual void DrawDebugData() override;


	virtual LightData GetLightData() override;

	virtual PixelFlagsLighting GetLightFlags() override;

	virtual Matrix GetTransformMatrix() override;

	virtual void Update(float dt) override;
};

