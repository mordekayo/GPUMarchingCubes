#pragma once

#include "Exports.h"

class Game;


class GAMEFRAMEWORK_API GameComponent
{
public:
	virtual ~GameComponent() = default;
	Game* game;

	GameComponent(Game* inGame)
	{
		game = inGame;
	}

	virtual void Initialize()				= 0;
	virtual void Update(float deltaTime)	= 0;
	virtual void Draw(float deltaTime)		= 0;
	virtual void DestroyResources()			= 0;


	virtual void Reload() { }
};
