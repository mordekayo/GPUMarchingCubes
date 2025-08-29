#include "../GameFramework/Camera.h"
#include "../GameFramework/CameraController.h"
#include "../GameFramework/PlaneComponent.h"
#include "../GameFramework/DebugRenderSystem.h"
#include "../GameFramework/ZActor.h"
#include "../GameFramework/UIRendererClass.h"
#include "../GameFramework/IRenderer2D.h"
#include "../GameFramework/ImGUI/imgui.h"
#include "RenderingPBRGame.h"

#include "CascadedDepthRenderer.h"
#include "RenderingSystemPBR.h"
#include "Lights.h"
#include "SkyRenderer.h"
#include "ZPostProcess.h"

#include "VoxelTree.h"


#define M_PI 3.14159265358979323846


AmbientLight* ambientLight;
DirectionalLight* directionalLight;
PointLight* pointLight;
SpotLight* spotLight;

PlaneComponent* plane;

GSampleData* sampleData = nullptr;

std::shared_ptr<ZAtmospherePostProc> postProcAtmos;
std::shared_ptr<ZAtmosCodingAdventure> codingAdventureAtmos;

std::vector<std::shared_ptr<ZActor>> Scene = {};

ZStaticMeshComponent* lastSphere = nullptr;
ZStaticMeshComponent* treeMesh = nullptr;
ZStaticMeshComponent* gunMesh = nullptr;
ZStaticMeshComponent* actorMesh = nullptr;



int DepthBias = 1300;
float DepthBiasClamp = 2;
float SlopeScaledDepthBias = 1.74f;


namespace RenderingPBRTut {

	void RenderingPBRGame::RegisterActor(std::shared_ptr<ZActor> actor)
	{
		Scene.push_back(actor);
		for (auto comp : actor->Components) {
			auto staticMeshComp = dynamic_cast<ZStaticMeshComponent*>(comp.get());
			if (staticMeshComp) {
				rendSys->RegisterMeshes(staticMeshComp->Meshes);
			}
		}
	}

	RenderingPBRGame::~RenderingPBRGame()
	{

	}


	void RenderingPBRGame::Initialize()
	{
		GameCamera = new Camera(this);
		controller = new CameraController(this, GameCamera);

		GameCamera->FarPlaneDistance = 5000.0f;

		DebugRender->SetCamera(GameCamera);

		isVSyncEnabled = false;

		rendSys = new RenderingSystemPBR(this);

#pragma region Scene
///*
		/*auto aaa = CreateNewStaticMeshActorAssimp("Content/wood_root/Aset_wood_root_M_rkswd_LOD0.fbx");
		aaa->FindComponentByClass<ZStaticMeshComponent>()->Meshes[0]->Materials[0].ReloadTextures(
			L"Content/wood_root/Aset_wood_root_M_rkswd_2K_Albedo.jpg",
			L"Content/wood_root/Aset_wood_root_M_rkswd_2K_Normal_LOD0.jpg",
			L"Content/wood_root/Aset_wood_root_M_rkswd_2K_Roughness.jpg",
			L"Content/DefaultMetallicMap.jpg");
		aaa->RootComponent->Transform.Rotation = Quaternion::CreateFromYawPitchRoll(-45, 0, -M_PI * 0.5f);
		aaa->RootComponent->Transform.Position = Vector3(-60, 20, 0);
		RegisterActor(aaa);
		
		auto actor3 = CreateNewStaticMeshActor("Content/Run To Rolling.fbx");
		actorMesh = actor3->FindComponentByClass<ZStaticMeshComponent>();
		actorMesh->Meshes[0]->Materials[0].RoughnessMult = 1.0f;
		actorMesh->Meshes[0]->Materials[0].MetallicMult = 1.0f;
		
		actor3->RootComponent->Transform.Rotation = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
		actor3->RootComponent->Transform.Position = Vector3(70, 0, -80);
		actor3->RootComponent->Transform.Scale = Vector3(0.3f, 0.3f, 0.3f);
		
		RegisterActor(actor3);
		
		auto actor = CreateNewStaticMeshActor("Content/wood_root/Aset_wood_root_M_rkswd_LOD0.fbx");
		treeMesh = actor->FindComponentByClass<ZStaticMeshComponent>();
		treeMesh->Meshes[0]->Materials[0].ReloadTextures(
			L"Content/wood_root/Aset_wood_root_M_rkswd_2K_Albedo.jpg", 
			L"Content/wood_root/Aset_wood_root_M_rkswd_2K_Normal_LOD0.jpg", 
			L"Content/wood_root/Aset_wood_root_M_rkswd_2K_Roughness.jpg",
			L"Content/DefaultMetallicMap.jpg");
		
		treeMesh->Meshes[0]->Materials[0].RoughnessMult = 1.0f;
		treeMesh->Meshes[0]->Materials[0].MetallicMult = 1.0f;
		
		actor->RootComponent->Transform.Rotation = Quaternion::CreateFromYawPitchRoll(0, 0, -M_PI * 0.5f);
		actor->RootComponent->Transform.Position = Vector3(-50, 20, -40);
		actor->RootComponent->Transform.Scale = Vector3(1, 1, 1);
		
		RegisterActor(actor);
		
		
		auto actor2 = CreateNewStaticMeshActor("Content/Cerberus_by_Andrew_Maximov/Cerberus_LP.fbx");
		gunMesh = actor2->FindComponentByClass<ZStaticMeshComponent>();
		gunMesh->Meshes[0]->Materials[0].ReloadTextures(
			L"Content/Cerberus_by_Andrew_Maximov/Textures/Cerberus_A.jpg",
			L"Content/Cerberus_by_Andrew_Maximov/Textures/Cerberus_N.jpg",
			L"Content/Cerberus_by_Andrew_Maximov/Textures/Cerberus_R.jpg",
			L"Content/Cerberus_by_Andrew_Maximov/Textures/Cerberus_M.jpg");
		
		gunMesh->Meshes[0]->Materials[0].RoughnessMult = 1.0f;
		gunMesh->Meshes[0]->Materials[0].MetallicMult = 1.0f;
		
		actor2->RootComponent->Transform.Rotation = Quaternion::CreateFromYawPitchRoll(0, -M_PI * 0.5f, 0);
		actor2->RootComponent->Transform.Position = Vector3(40, 60, -150);
		actor2->RootComponent->Transform.Scale = Vector3(1, 1, 1);
		
		RegisterActor(actor2);
		
		
		auto floor = CreateNewStaticMeshActor("Content/SM_MERGED_Floor.fbx");
		floor->RootComponent->Transform.Rotation = Quaternion::CreateFromYawPitchRoll(0, -M_PI*0.5f, 0);
		floor->RootComponent->Transform.Position = Vector3(-150, -10, -150);
		floor->RootComponent->Transform.Scale = Vector3(0.1f, 0.1f, 0.1f);
		RegisterActor(floor);

		auto inside = CreateNewStaticMeshActor("Content/SM_MERGED_inside.fbx");
		inside->RootComponent->Transform.Rotation = Quaternion::CreateFromYawPitchRoll(0, -M_PI * 0.5f, 0);
		inside->RootComponent->Transform.Position = Vector3(-150, -10, -150);
		inside->RootComponent->Transform.Scale = Vector3(0.1f, 0.1f, 0.1f);
		RegisterActor(inside);*/

		
	/*int rows = 4;
	int cols = 4;
	for (int i = 0; i < rows; i++) {
		float istep = 1.0f / (rows - 1.0f) * i;
		std::cout << "\n\n" << istep << "\n\n";
		for (size_t j = 0; j < cols; j++) {
			float jstep = 1.0f / (cols - 1.0f) * j;
	
			auto sphere = CreateNewStaticMeshActor("Content/HighPolyGeoSphere.FBX");
	
			int row = i;
			int col = j;
	
			sphere->RootComponent->Transform.Scale = Vector3(10, 10, 10);
			sphere->RootComponent->Transform.Position = Vector3(25 * row, 10, 25 * col);
	
			lastSphere = sphere->FindComponentByClass<ZStaticMeshComponent>();
			lastSphere->Meshes[0]->Materials[0].RoughnessMult = istep;
			lastSphere->Meshes[0]->Materials[0].MetallicMult = jstep;
	
			RegisterActor(sphere);
		}
	}*/
//*/
#pragma endregion Scene


		//auto sphere = CreateNewStaticMeshActor("Content/HighPolyGeoSphere.FBX");
		//
		//sphere->RootComponent->Transform.Scale = Vector3(30, 30, 30);
		//sphere->RootComponent->Transform.Position = Vector3(0, 0, 0);
		//
		//lastSphere = sphere->FindComponentByClass<ZStaticMeshComponent>();
		//lastSphere->Meshes[0]->Materials[0].RoughnessMult = 1.0f;
		//lastSphere->Meshes[0]->Materials[0].MetallicMult = 1.0f;
		//
		//RegisterActor(sphere);

		controller->CameraPosition = Vector3(0, 30, 0);


		
		auto colorWhite = Color(1.0f, 1.0f, 1.0f, 1.0f);

		ambientLight = new AmbientLight(0.05f, 0.05f, colorWhite);
		directionalLight = new DirectionalLight(DirectX::SimpleMath::Vector3(0.416f, 0.536f, 0.734f), 1.0f, colorWhite);
		rendSys->RegisterLight(ambientLight);
		rendSys->RegisterLight(directionalLight);
		
		pointLight = new PointLight(Vector3(0, 100, 200), 1000, 500, 10.0f, colorWhite);
		//rendSys->RegisterLight(pointLight);

		spotLight = new SpotLight(
			DirectX::SimpleMath::Vector3(0, 0, 300),
			DirectX::SimpleMath::Vector3(0, 0, -1),
			450,
			static_cast<float>(M_PI * 0.25f * 0.1f),
			static_cast<float>(M_PI * 0.25f * 0.3f + 0.01f),
			10.0f, 300.0f, colorWhite);



		//directionalLight->PosDir.y = 0.75f;
		//if (directionalLight->PosDir.y > 0.95f) directionalLight->PosDir.y = 0.0f;
		directionalLight->PosDir.Normalize();
		rendSys->skyRend->Direction = directionalLight->PosDir;
		rendSys->skyRend->bNeedToUpdateSky = true;

		rendSys->ResetShadowRasterizerState(DepthBias, DepthBiasClamp, SlopeScaledDepthBias);


		postProcAtmos = std::make_shared<ZAtmospherePostProc>(*directionalLight);
		postProcAtmos->Init();
		
		codingAdventureAtmos = std::make_shared<ZAtmosCodingAdventure>(*directionalLight);
		codingAdventureAtmos->Init();
		
		//rendSys->RegisterPostProcess(codingAdventureAtmos);
	}


	void Unproject( Vector3 vector, float x, float y, float width, float height, float minZ, float maxZ, Matrix worldViewProjection, Vector3& result)
	{
		Vector3 coordinate;
		Matrix result1;
		worldViewProjection.Invert(result1);

		coordinate.x = (float)(((double)vector.x - (double)x) / (double)width * 2.0 - 1.0);
		coordinate.y = (float)-(((double)vector.y - (double)y) / (double)height * 2.0 - 1.0);
		coordinate.z = (float)(((double)vector.z - (double)minZ) / ((double)maxZ - (double)minZ));
		Vector3::Transform(coordinate, result1, result);
	}


	Vector3 wNear = Vector3::Zero;
	Vector3 wFar = Vector3::Zero;
	Vector3 point = Vector3::Zero;

	bool lastPressed = false;
	void RenderingPBRGame::Update(float deltaTime)
	{
		controller->Update(deltaTime);

		rendSys->Update(deltaTime);

		//std::wstring str = L"sdasdasdsa";
		//Renderer2D->DrawOnScreenMessage(str);

		if (InputDevice->IsKeyDown(Keys::L)) {
			directionalLight->PosDir = -GameCamera->GetForwardVector();
		}

		if (InputDevice->IsKeyDown(Keys::R) && !lastPressed) {
			postProcAtmos->Reload();
			std::cout << "Reload\n";
		}
		lastPressed = InputDevice->IsKeyDown(Keys::R);

		if (InputDevice->IsKeyDown(Keys::Escape)) 
			Exit();

		static float requestCountDown = 2.0f;

		requestCountDown -= deltaTime;
		requestCountDown = requestCountDown < -1.0f ? -1.0f : requestCountDown;

		if ((InputDevice->IsKeyDown(Keys::MiddleButton) || InputDevice->IsKeyDown(Keys::RightButton)) && requestCountDown < 0)
		{
			const bool isSub = InputDevice->IsKeyDown(Keys::RightButton);
			rendSys->RequestScreenSample(InputDevice->MousePosition.x, InputDevice->MousePosition.y, [&, isSub](const GSampleData& data)
			{
				if(!sampleData)
					sampleData = new GSampleData{};
				*sampleData = data;
				std::cout << sampleData->WorldPosition.x << " " << sampleData->WorldPosition.y << " " << sampleData->WorldPosition.z << "\n";
				//rendSys->volRenderer->AddSdfSphere(sampleData->WorldPosition, 0.25f, isSub);

					rendSys->voxelTree->AddSdfSphere(sampleData->WorldPosition, 20.5f, isSub);
			});
			requestCountDown = 0.01f;
		}


		Game::Update(deltaTime);

		for (auto& actor : Scene) {
			actor->Update(deltaTime);
		}
	}


	bool showGBuf = false;
	Vector3 pos = Vector3(0.0f, 0.0f, 50.0f);
	float ThetaUmbra = static_cast<float>(M_PI * 0.25 * 0.1);
	float PhiPenumbra = static_cast<float>(M_PI * 0.25 * 0.3 + 0.01);


	void RenderingPBRGame::PostDraw(float deltaTime)
	{
		DebugRender->Clear();

		rendSys->voxelTree->DrawDebug();

		if (showGBuf) {
			DebugRender->DrawTextureOnScreen(rendSys->gBuffer.DiffuseSRV, 10, 10, 512, 288);
			DebugRender->DrawTextureOnScreen(rendSys->gBuffer.AccumulationSRV, 532, 10, 512, 288);
			DebugRender->DrawTextureOnScreen(rendSys->gBuffer.NormalSRV, 10, 308, 512, 288);
			DebugRender->DrawTextureOnScreen(rendSys->gBuffer.MetRougAoIdSRV,	532, 308, 512, 288);
		}

		//DebugRender->DrawLine(wNear, wFar, Color(1.0f, 1.0f, 0.0f, 1.0f));
		DebugRender->DrawPoint(point, 10);

		if(sampleData)
		{
			DebugRender->DrawPoint(sampleData->WorldPosition, 1);
			DebugRender->DrawLine(sampleData->WorldPosition, sampleData->WorldPosition + sampleData->WorldNormal * 3, Color{ 0.0f, 0.0f, 1.0f, 1.0f });
		}

		UIRender->BeforeLayout(deltaTime);

		ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Once);

		ImGui::Begin("Settings");
		if (ImGui::Button("Show GBuffer"))	showGBuf = !showGBuf;
		if (ImGui::Button("Toggle vsync"))	isVSyncEnabled = !isVSyncEnabled;
		ImGui::SameLine(); if (ImGui::Button("Show DSphere"))	pointLight->isDrawDebug = !pointLight->isDrawDebug;
		ImGui::SameLine(); if (ImGui::Button("Show DSpot"))		spotLight->isDrawDebug = !spotLight->isDrawDebug;

		ImGui::DragFloat("Ambient Intensity", &ambientLight->Intensity, 0.001f, 0.0f, 1.0f);

		if (ImGui::TreeNode("Material"))
		{
			ImGui::DragFloat("Metallic", &lastSphere->Meshes[0]->Materials[0].MetallicMult, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Roughness", &lastSphere->Meshes[0]->Materials[0].RoughnessMult, 0.1f, 0.0f, 1.0f);

			ImGui::Separator();

			ImGui::DragFloat("Tree Metallic", &treeMesh->Meshes[0]->Materials[0].MetallicMult, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Tree Roughness", &treeMesh->Meshes[0]->Materials[0].RoughnessMult, 0.1f, 0.0f, 1.0f);
			
			ImGui::TreePop();
			ImGui::Separator();
		}

		if (ImGui::TreeNode("Ambient light"))
		{
			ImGui::DragFloat("Intensity", &ambientLight->Intensity, 0.1f, 0, 1000);
			ImGui::DragFloat("SpecIntensity", &ambientLight->SpecIntensity, 0.1f, 0, 1000);

			ImGui::TreePop();
			ImGui::Separator();
		}

		if (ImGui::TreeNode("Directional light"))
		{
			ImGui::DragFloat("Intensity", &directionalLight->Intensity, 0.1f, 0, 1000);
			if(ImGui::DragFloat3("Length", &directionalLight->PosDir.x, 0.1, -1.0f, 1.0f))
			{
				directionalLight->PosDir.Normalize();

				rendSys->skyRend->Direction = directionalLight->PosDir;
				rendSys->skyRend->bNeedToUpdateSky = true;
			}
			ImGui::TreePop();
			ImGui::Separator();
		}

		if (ImGui::TreeNode("Point light"))
		{
			ImGui::DragFloat("Intensity", &pointLight->Intensity, 0.1f, 0, 250);
			//ImGui.DragFloat("Atten Rad", ref pointLight.AttenRadius, 1, 0, 500);
			ImGui::DragFloat("Rad", &pointLight->Radius, 0.1f, 0, 1000);
			ImGui::DragFloat3("Point Pos", &pos.x);

			ImGui::TreePop();
			ImGui::Separator();
		}

		if (ImGui::TreeNode("Spot light"))
		{
			ImGui::DragFloat("Intensity", &spotLight->Intensity, 0.1f, 0, 100);
			ImGui::DragFloat("Length", &spotLight->Length, 1, 1, 1000);
			ImGui::DragFloat("Falloff", &spotLight->Falloff, 0.001f, 0, 100);
			ImGui::DragFloat("ThetaUmbra", &ThetaUmbra, 0.001f, 0, M_PI * 0.5);
			ImGui::DragFloat("PhiPenumbra", &PhiPenumbra, 0.001f, 0, M_PI * 0.5);

			ImGui::TreePop();
			ImGui::Separator();
		}


		if (ImGui::TreeNode("ToneMapping"))
		{
			if (ImGui::Button("CopyColor"))		rendSys->ToneFlagPs = PSToneMapFlags::CopyColor;
			if (ImGui::Button("Simple"))		rendSys->ToneFlagPs = PSToneMapFlags::Simple;
			if (ImGui::Button("WithLum"))		rendSys->ToneFlagPs = PSToneMapFlags::WithLum;
			if (ImGui::Button("WithLumEx"))		rendSys->ToneFlagPs = PSToneMapFlags::WithLumEx;
			ImGui::DragFloat("Luminance", &rendSys->Luminance, 0.001f, 0, 30);

			ImGui::TreePop();
			ImGui::Separator();
		}
		if (ImGui::TreeNode("Depth Rasterizer"))
		{
			if (ImGui::DragInt("DepthBias", &DepthBias, 50, -500000, 500000)
				|| ImGui::DragFloat("DepthBiasClamp", &DepthBiasClamp, 0.01f, -100.0f, 100.0f)
				|| ImGui::DragFloat("SlopeScaledDepthBias", &SlopeScaledDepthBias, 0.01f, -100.0f, 100.0f))
			{
				rendSys->ResetShadowRasterizerState(DepthBias, DepthBiasClamp, SlopeScaledDepthBias);
			}
			ImGui::TreePop();
			ImGui::Separator();
		}
		if (ImGui::TreeNode("Atmosphere"))
		{
			ImGui::Separator();
			ImGui::Text("Rayleigh Scattering");
			ImGui::DragFloat3("Rayleigh Coefs", &postProcAtmos->data.beta_R.x, 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat("Rayleigh Scale Height", &postProcAtmos->data.H_R, 0.01f, 0.0f, 15.0f);
			ImGui::Separator();
			ImGui::Text("Mie Scattering");
			ImGui::DragFloat("Mie Coefs", &postProcAtmos->data.beta_M, 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat("Mie Scale Height", &postProcAtmos->data.H_M, 0.01f, 0.0f, 15.0f);
			ImGui::DragFloat("Mie Anisotropy", &postProcAtmos->data.g, 0.001f, 0.0f, 2.0f);
			ImGui::Separator();
			ImGui::Text("Earth params"); ImGui::SameLine(); if (ImGui::Button("Set camera to earth surface", { 50, 20 })) controller->CameraPosition = {0, postProcAtmos->data.R_e + 1, 0};
			ImGui::DragFloat("Earth Radius, m", &postProcAtmos->data.R_e, 10.0f, 30.0f, 15000.0f);
			ImGui::DragFloat("Earth Atmos, m", &postProcAtmos->data.R_a, 10.0f, 60.0f, 30000.0f);

			ImGui::TreePop();
			ImGui::Separator();
		}
		if (ImGui::TreeNode("CodingAdventureAtmosphere"))
		{
			ImGui::Separator();
			ImGui::DragFloat("Density Falloff", &codingAdventureAtmos->data.DensityFalloff, 0.01f, -10.0f, 25.0f);
			ImGui::DragFloat("Earth Radius, m", &codingAdventureAtmos->data.PlanetRadius, 10.0f, 20.0f, 15000.0f);
			ImGui::DragFloat("Earth Atmos, m", &codingAdventureAtmos->data.AtmosphereRadius, 10.0f, 40.0f, 30000.0f);
			ImGui::DragInt("Num Scattering Points", &codingAdventureAtmos->data.numScatteringPoints, 1, 2, 30);
			ImGui::DragInt("Num Optical Points", &codingAdventureAtmos->data.numOpticalDepthPoints, 1, 2, 30);

			ImGui::TreePop();
			ImGui::Separator();
		}
		if(ImGui::TreeNode("Point Volume Draw"))
		{
			ImGui::Separator();

			ImGui::DragFloat("Scale", &rendSys->volRenderer->constData.ScaleParticleSizeThreshold.x, 0.1f, 0.0f, 500.0f);
			ImGui::DragFloat("Particle Size", &rendSys->volRenderer->constData.ScaleParticleSizeThreshold.y, 0.01f, 0.0f, 25.0f);
			ImGui::DragFloat("Threshold", &rendSys->volRenderer->constData.ScaleParticleSizeThreshold.z, 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat3("Offset", &rendSys->volRenderer->Offset.x, 0.1f, -10, 10);
			ImGui::DragInt("Cunks to draw", &rendSys->volRenderer->chunksToDraw, 1, 0, rendSys->volRenderer->chunks.size() - 1);
			ImGui::DragFloat("Noise Scale", &rendSys->volRenderer->computeConstData.SizeXYZScale.w, 0.01f, 0.001f, 2.0f);
			
			ImGui::TreePop();
			ImGui::Separator();
		}

		if (ImGui::TreeNode("Fog"))
		{
			ImGui::Separator();

			ImGui::DragFloat3("Offset", &rendSys->fogData.FogColor.x, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("FogStartDist", &rendSys->fogData.FogStartDist, 0.1f, 0.0f, 500.0f);
			ImGui::DragFloat3("FogHighlightColor", &rendSys->fogData.FogHighlightColor.x, 0.1f, -10, 10);
			ImGui::DragFloat("FogGlobalDensity", &rendSys->fogData.FogGlobalDensity, 0.01f, 0.0f, 25.0f);
			ImGui::DragFloat("FogHeightFalloff", &rendSys->fogData.FogHeightFalloff, 0.01f, -10.0f, 10.0f);

			ImGui::TreePop();
			ImGui::Separator();
		}

		//static float camPosZ = 0;
		//static float camPosY = 0;
		//static float camPosX = 0;
		//if(ImGui::SliderFloat("PosZ", &camPosZ, -10000.0f, 10000.0f) || ImGui::SliderFloat("PosY", &camPosY, -10000.0f, 10000.0f) || ImGui::SliderFloat("PosX", &camPosX, -10000.0f, 10000.0f))
		//	localCam.ViewMatrix = Matrix::CreateLookAt(Vector3(camPosX, camPosY, camPosZ), Vector3(camPosX, camPosY, camPosZ) + Vector3::Forward, Vector3::Up);
		ImGui::SliderFloat("Falloff", &rendSys->voxelTree->falloff, 0.01f, 10.0f);

		ImGui::End();

		UIRender->AfterLayout();
		

		spotLight->SetAngles(ThetaUmbra, PhiPenumbra);

		ThetaUmbra = spotLight->GetThetaUmbra();
		PhiPenumbra = spotLight->GetPhiPenumbra();

		pointLight->PosDir = Vector3(pos.x, pos.y, pos.z);


		Renderer2D->Draw();
		Renderer2D->ClearMessages();
	}

	void RenderingPBRGame::Draw(float deltaTime)
	{
		rendSys->Draw(deltaTime);
	}
}
