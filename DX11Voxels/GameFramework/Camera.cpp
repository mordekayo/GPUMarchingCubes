#include "Camera.h"
#include "Game.h"

#define M_PI 3.14159265358979323846

Camera::Camera(Game* inGame) : game(inGame)
{
	ViewMatrix = DirectX::SimpleMath::Matrix::Identity;

	Fov = (float)M_PI * 0.35f;

	UpdateProjectionMatrix();
}

Camera::~Camera()
{
}

DirectX::SimpleMath::Matrix Camera::GetCameraMatrix() const
{
	return ViewMatrix * ProjMatrix;
}

DirectX::SimpleMath::Vector3 Camera::GetPosition() const
{
	DirectX::SimpleMath::Matrix inv;
	ViewMatrix.Invert(inv);
	return inv.Translation();
}

DirectX::SimpleMath::Vector3 Camera::GetForwardVector() const
{
	DirectX::SimpleMath::Matrix inv;
	ViewMatrix.Invert(inv);
	return inv.Forward();
}

DirectX::SimpleMath::Vector3 Camera::GetUpVector() const
{
	DirectX::SimpleMath::Matrix inv;
	ViewMatrix.Invert(inv);
	return inv.Up();
}

void Camera::UpdateProjectionMatrix()
{
	AspectRatio = (float)game->Display->ClientWidth / (float)game->Display->ClientHeight;

	ProjMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		Fov, 
		AspectRatio, 
		NearPlaneDistance,
		FarPlaneDistance);
}

#undef M_PI
