#pragma once

#include <string>
#include "../ZMathLib/SimpleMath.h"
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_internal.h>
#include <mono/metadata/object.h>


namespace ImGuiSharp
{
	inline bool Begin(MonoString* inName)
	{
		std::string name = mono_string_to_utf8(inName);
		return ImGui::Begin(name.c_str());
	}
	inline void End() { ImGui::End(); }

	inline bool DragFloat4(MonoString* inLabel, DirectX::SimpleMath::Vector4& float4, float speed, float min, float max)
	{
		std::string label = mono_string_to_utf8(inLabel);

		return ImGui::DragFloat4(label.c_str(), &float4.x, speed, min, max);
	}

	
	inline void InitGlue()
	{
		mono_add_internal_call("SharpScriptCore.ImGUI::Begin(string)", ImGuiSharp::Begin);
		mono_add_internal_call("SharpScriptCore.ImGUI::End", ImGuiSharp::End);
		mono_add_internal_call("SharpScriptCore.ImGUI::DragFloat4", ImGuiSharp::DragFloat4);
	}

}

