#pragma once

#include "Exports.h"


#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"



class GAMEFRAMEWORK_API ScriptingSystem
{
public:
	static sol::state lua;

	ScriptingSystem();

	void RunScriptFile(const char* fileName);
	void RunScriptString(const char* scriptString);
	void RunScriptString(const char* scriptString, const sol::environment& env);

	template<class C, typename... Args>
	void RegisterUserClass(const char* objName, Args&&... args)
	{
		lua.new_usertype<C>(objName, std::forward<Args>(args)...);
	}

	template<class C>
	void RegisterObject(const char* objName, const C& object)
	{
		lua[objName] = object;
	}

	static bool Init();
	static bool isInitialized;
};

