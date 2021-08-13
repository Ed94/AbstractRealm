// Parent
// ImGui.hpp



namespace SAL::Imgui
{
	template<typename Type>
	bool Table2C::NestedTree(Type _name)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		return TreeNode(_name);
	}	

	template<typename Type>
	bool Table2C::NestedCollapsingHeader(Type _name)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		return CollapsingHeader(_name);
	}

	template<typename Type, typename TypeVal>
	void Table2C::Entry(Type _name, TypeVal _value)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		Text(_name);

		ImGui::TableNextColumn();

		using RawType = RawType<TypeVal>;

		if constexpr (IsStringRelated<TypeVal>())
		{
			if constexpr (IsSameTypeCV<TypeVal, RoCStr>())
			{
				ImGui::Text(_value);
			}
			else if constexpr (IsSameTypeCV<TypeVal, String>())
			{
				ImGui::Text(_value.c_str());
			}
			else if constexpr (IsSameTypeCV<TypeVal, StringView>())
			{
				ImGui::Text(_value.data());
			}
		}

		if constexpr (IsEnum<TypeVal>())
		{
			ImGui::Text(nameOf(_value).data());
		}

		if constexpr (IsArithmetic<TypeVal>())
		{
			ImGui::Text(ToString(_value).c_str());
		}
	}
}