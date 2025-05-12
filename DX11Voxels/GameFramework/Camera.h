#pragma once

#include "Exports.h"
#include "pch.h"

class Game;

class GAMEFRAMEWORK_API Camera
{
	Game* game;
	
public:
	DirectX::SimpleMath::Matrix ViewMatrix;
	DirectX::SimpleMath::Matrix ProjMatrix;

	float FarPlaneDistance	= 10000.0f;
	float NearPlaneDistance = 0.1f;
	float Fov			= 1.67f;
	float AspectRatio	= 1.0f;

	Camera(Game* game);
	~Camera();

	DirectX::SimpleMath::Matrix GetCameraMatrix() const;

	DirectX::SimpleMath::Vector3 GetPosition() const;
	DirectX::SimpleMath::Vector3 GetForwardVector() const;
	DirectX::SimpleMath::Vector3 GetUpVector() const;



	void UpdateProjectionMatrix();
};

