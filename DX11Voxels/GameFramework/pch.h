// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#pragma warning(disable : 4275)
#pragma warning(disable : 4267)
#pragma warning (disable: 4251)

// add headers that you want to pre-compile here
#include "framework.h"
#include <wrl.h>
#include <string>
#include <vector>
#include <chrono>


// All DirectX stuff
#include <d3d.h>
#include <d3d11.h>
#include <d3d11_2.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXColors.h>

#include "../ZMathLib/SimpleMath.h"

#include "Delegates.h"

#include "StringHashCompileTime.h"
#include "CycleProfiler.h"


#define ZCHECK(exp) if(FAILED(exp)) { printf("Check failed at file: %s at line %i", __FILE__, __LINE__); throw new std::exception(); }

#endif //PCH_H
