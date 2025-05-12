#include "ScriptingSystem.h"

#include "sol_ImGui.h"

sol::state ScriptingSystem::lua;
bool ScriptingSystem::isInitialized = ScriptingSystem::Init();

ScriptingSystem::ScriptingSystem()
{
	lua.do_string("package.path = '../../Misc/knife/?.lua;'..package.path");
}

void ScriptingSystem::RunScriptFile(const char* fileName)
{
	
}


void CheckError(const sol::protected_function_result& ret)
{
	if (!ret.valid()) {
		sol::error err = ret;
		std::string what = err.what();
		std::cout << what << std::endl;
	}
}


void ScriptingSystem::RunScriptString(const char* scriptString)
{
	auto ret = lua.do_string(scriptString);
	CheckError(ret);
}


void ScriptingSystem::RunScriptString(const char* scriptString, const sol::environment& env)
{
	auto ret = lua.do_string(scriptString, env);
	CheckError(ret);
}


bool ScriptingSystem::Init()
{
	lua.open_libraries();
	sol_ImGui::Init(lua);

	return true;
}
