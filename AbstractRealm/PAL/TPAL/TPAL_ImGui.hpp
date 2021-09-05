 #pragma once

#include "OSAL/OSAL_Platform.hpp"
#include "LAL/LAL.hpp"
#include "Meta/Config/HAL_Config.hpp"
#include "Meta/Config/OSAL_Config.hpp"
#include "OSAL/OSAL_Windowing.hpp"

#include "imgui/imgui.h"


namespace TPAL::Imgui
{
	using namespace LAL;

	using IO = ImGuiIO;

	using WindowCallback = Function<void()>;

	//struct ptr<ImGuiContext> CreateContext(ptr<struct ImFontAtlas> shared_font_atlas);

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

	void Queue(RoCStr _windowName, WindowCallback _callback_in);
	void Dequeue(RoCStr _windowName, WindowCallback _callback_in);

	void Dirty_DoSurfaceStuff(ptr<OSAL::Window> _window_in);
	void BindToPlatformAndRenderer(ptr<OSAL::Window> _window_in);
	void Initialize(ptr<OSAL::Window> _window_in);

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

	class Table
	{
	public:

		static void EndRecord();
	};

	// 2 Column Table (Name, Value)
	class Table2C : Table
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

		using Table::EndRecord;

	protected:

		String name;
		Flags  flags;
	};

	class Table4C : Table
	{
	public:

		template<typename Type>
		static bool Record(Type _nameC1, Type _nameC2, Type _nameC3, Type _nameC4);

		template<typename TypeC1, typename TypeC2, typename TypeC3, typename TypeC4>
		static void Entry(TypeC1 _valueC1, TypeC2 _valueC2, TypeC3 _valueC3, TypeC4 _valueC4);

		using Table::EndRecord;
	};

	class Table5C : Table
	{
	public:

		template<typename Type>
		static bool Record(Type _nameC1, Type _nameC2, Type _nameC3, Type _nameC4, Type _nameC5);

		template<typename TypeC1, typename TypeC2, typename TypeC3, typename TypeC4, typename TypeC5>
		static void Entry(TypeC1 _valueC1, TypeC2 _valueC2, TypeC3 _valueC3, TypeC4 _valueC4, TypeC5 _valueC5);

		using Table::EndRecord;
	};
}

#include "TPAL_ImGui.tpp"


