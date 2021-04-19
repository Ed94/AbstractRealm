#pragma once



// Includes

#include "LAL/LAL.hpp"
#include "Meta/Config/HAL_Config.hpp"
#include "Meta/Config/OSAL_Config.hpp"
#include "OSAL/OSAL_Platform.hpp"
#include "OSAL/OSAL_Windowing.hpp"

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "imgui/imgui.h"



namespace SAL::Imgui
{
	using namespace LAL;

	using IO = ImGuiIO;

	using WindowCallback = Function<void()>;

	using ImGui::CreateContext  ;
	using ImGui::GetIO          ;
	using ImGui::Render         ;
	using ImGui::StyleColorsDark;
	using ImGui::BeginChild;
	using ImGui::EndChild;

	namespace PlatformBackend
	{
		//eGlobal bool GLFW_InstallCallbacks;
	}

	void Queue(RoCStr _windowName, WindowCallback _callback);
	void Dequeue(RoCStr _windowName, WindowCallback _callback);

	void Dirty_DoSurfaceStuff(ptr<OSAL::Window> _window);
	void BindToPlatformAndRenderer(ptr<OSAL::Window> _window);
	void Initialize(ptr<OSAL::Window> _window);

	void Deinitialize();
	void MakeFrame();
	void SetupFonts();
	void SetupGPU_Interface();
	void VerifyVersion();

	void Text(RoCStr _cStr);
	void Text(const StringView& _view);
	void Text(const String& _string);

	bool TreeNode(RoCStr _cStr);
	bool TreeNode(const StringView& _view);
	bool TreeNode(const String& _string);

	using ImGui::TreePop;

	bool CollapsingHeader(RoCStr _cStr);
	bool CollapsingHeader(const StringView& _view);
	bool CollapsingHeader(const String& _string);

	// 2 Column Table (Name, Value)
	class Table2C 
	{
	public:

		using Flags = ImGuiTableFlags;

		Table2C();

		Table2C(      RoCStr  _name);
		Table2C(const String& _name);

		Table2C(Flags _flags);

		Table2C(      RoCStr  _name, Flags _flags);
		Table2C(const String& _name, Flags _flags);

		static bool Record();

		template<typename Type> static bool NestedTree(Type _name);
		template<typename Type> static bool NestedCollapsingHeader(Type _name);

		template<typename Type, typename TypeVal>
		static void Entry(Type _name, TypeVal _value);

		static void EndRecord();

	protected:

		String name;
		Flags  flags;
	};
}

#include "ImGui_SAL.tpp"


