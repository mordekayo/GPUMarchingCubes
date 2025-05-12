#pragma once
#include "Exports.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoDomain MonoDomain;
}


namespace ZGame
{

	class GAMEFRAMEWORK_API CSharpScriptingSystem
	{
	public:
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;


		CSharpScriptingSystem();

		void Init();
		void Reload();

		void PrintAssemblyTypes(MonoAssembly* assembly);
	};

}
