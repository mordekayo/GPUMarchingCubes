#pragma once
#include "../GameFramework/Game.h"
#include <string>

class Camera;
class CameraController;
class RenderingSystemPBR;
class ZActor;

namespace RenderingPBRTut {

	class RenderingPBRGame : public Game
	{
		Camera* cam = nullptr;
		CameraController* controller = nullptr;


	public:
		RenderingPBRGame(std::wstring name) : Game(name)
		{
		}

		RenderingSystemPBR* rendSys = nullptr;

		void RegisterActor(std::shared_ptr<ZActor> actor);

		virtual ~RenderingPBRGame();

	protected:
		virtual void Initialize() override;
		virtual void Update(float deltaTime) override;
		virtual void PostDraw(float deltaTime) override;
		virtual void Draw(float deltaTime) override;
	};

}

