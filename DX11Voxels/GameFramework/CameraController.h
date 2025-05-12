#pragma once

#include "Exports.h"
#include "InputDevice.h"

class Game;
class Camera;


class GAMEFRAMEWORK_API CameraController
{
	Game* game;
public:
	float Yaw;
	float Pitch;

	float VelocityMagnitude = 100.0f;
	float MouseSensitivity	= 1.0f;

	Camera* Camera;
	DirectX::SimpleMath::Vector3 CameraPosition;

	CameraController(Game* inGame, ::Camera* inCamera);
	void Update(float deltaTime);

protected:
	 
	void OnMouseMove(const InputDevice::MouseMoveEventArgs& args);
};

