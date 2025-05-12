// Lesson05Shadows.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "RenderingPBRGame.h"
#pragma comment(lib, "d3dcompiler.lib")


int main()
{
	auto* tGame = new RenderingPBRTut::RenderingPBRGame(L"RenderingPBRGame");
	tGame->Run(1024, 768);

	delete tGame;
}
