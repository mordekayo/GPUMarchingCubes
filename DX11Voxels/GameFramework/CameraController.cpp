#include "pch.h"
#include "CameraController.h"
#include "Game.h"
#include "Camera.h"


using namespace DirectX::SimpleMath;


CameraController::CameraController(Game* inGame, ::Camera* inCamera)
{
	game = inGame;
	Camera = inCamera;

	Yaw = 0;
	Pitch = 0;
	game->InputDevice->MouseMove.AddRaw(this, &CameraController::OnMouseMove);
}

void CameraController::Update(float deltaTime)
{
	if (!game->IsMainWindowActive()) return;

	auto input = game->InputDevice;

	auto rotMat = Matrix::CreateFromYawPitchRoll(Yaw, Pitch, 0);

	auto velDirection = Vector3::Zero;
	if (input->IsKeyDown(Keys::W)) velDirection += Vector3(1.0f, 0.0f, 0.0f);
	if (input->IsKeyDown(Keys::S)) velDirection += Vector3(-1.0f, 0.0f, 0.0f);
	if (input->IsKeyDown(Keys::A)) velDirection += Vector3(0.0f, 0.0f, -1.0f);
	if (input->IsKeyDown(Keys::D)) velDirection += Vector3(0.0f, 0.0f, 1.0f);

	if (input->IsKeyDown(Keys::Space))	velDirection += Vector3(0.0f, 1.0f, 0.0f);
	if (input->IsKeyDown(Keys::C))		velDirection += Vector3(0.0f, -1.0f, 0.0f);

	velDirection.Normalize();

	auto velDir = rotMat.Forward() * velDirection.x
	+ Vector3::Up * velDirection.y
	+ rotMat.Right() * velDirection.z;
	if (velDir.Length() != 0) {
		velDir.Normalize();
	}

	CameraPosition = CameraPosition + velDir * VelocityMagnitude * deltaTime;
	Camera->ViewMatrix = Matrix::CreateLookAt(CameraPosition, CameraPosition + rotMat.Forward(), rotMat.Up());
	Camera->UpdateProjectionMatrix();
}

void CameraController::OnMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	if (!game->IsMainWindowActive()) return;
	if (game->InputDevice->IsKeyDown(Keys::LeftShift)) return;

	Yaw		-= args.Offset.x * 0.003f * MouseSensitivity;
	Pitch	-= args.Offset.y * 0.003f * MouseSensitivity;

	if (args.WheelDelta > 0) VelocityMagnitude *= 1.1f;
	if (args.WheelDelta < 0) VelocityMagnitude *= 0.9f;
}
