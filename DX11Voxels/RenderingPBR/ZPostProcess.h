#pragma once
#include <filesystem>
#include <d3d11.h>
#include "../ZMathLib/SimpleMath.h"


struct GBuffer;
class DirectionalLight;

struct ZPostProcess
{
#pragma region Params 
	std::filesystem::path pixelShaderPath;
	std::string  psEntryPoint;
	

	ID3D11VertexShader* vsScreenQuad = nullptr;
	ID3D11PixelShader* ps = nullptr;

	ID3D11Buffer* constBuf = nullptr;

	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11RenderTargetView* rtv = nullptr;

	bool CopyToAcc = true;

#pragma endregion

	virtual void Init();
	
	virtual void Update(float dTime);
	virtual void Draw(const GBuffer& gBuffer, float dTime);

	virtual void Reload();

	virtual ~ZPostProcess() {}
};



struct ZAtmospherePostProc : public ZPostProcess
{
#pragma pack(push, 4)
	struct AtmosConstData
	{
		DirectX::SimpleMath::Matrix invVP;

		DirectX::SimpleMath::Vector3 viewPos;   // Position of the viewer
		float g;		// Mie scattering direction - anisotropy of the medium

		DirectX::SimpleMath::Vector3 sunPos;    // Position of the sun, light direction
		float I_sun;    // Intensity of the sun

		// Number of samples along the view ray and light ray
		int viewSamples;
		int lightSamples;
		float R_e;      // Radius of the planet [m]
		float R_a;      // Radius of the atmosphere [m]

		DirectX::SimpleMath::Vector3  beta_R;   // Rayleigh scattering coefficient
		float beta_M;   // Mie scattering coefficient

		float H_R;      // Rayleigh scale height
		float H_M;      // Mie scale height

		DirectX::SimpleMath::Vector2 dummy;
	};
#pragma pack (pop)

	AtmosConstData data;

	const DirectionalLight& dirLight;

	ZAtmospherePostProc(const DirectionalLight& inDirLight) : dirLight(inDirLight) {}

	virtual void Init() override;
	virtual void Update(float dTime) override;
};


struct ZAtmosCodingAdventure : public ZPostProcess
{
#pragma pack(push, 4)
	struct CodingAdventureData
	{
		DirectX::SimpleMath::Matrix invVP;
		DirectX::SimpleMath::Vector3 viewPos;   // Position of the viewer
		float PlanetRadius;

		DirectX::SimpleMath::Vector3 PlanetCenter;
		float AtmosphereRadius;

		DirectX::SimpleMath::Vector3 sunDir;    // Position of the sun, light direction
		float I_sun;    // Intensity of the sun

		float DensityFalloff;
		int numScatteringPoints;
		int numOpticalDepthPoints;
		float dummy0;
	};
#pragma pack (pop)

	CodingAdventureData data;

	const DirectionalLight& dirLight;

	ZAtmosCodingAdventure(const DirectionalLight& inDirLight) : dirLight(inDirLight) {}

	virtual void Init() override;
	virtual void Update(float dTime) override;
};

