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

	template<typename Type>
	bool Table4C::Record(Type _nameC1, Type _nameC2, Type _nameC3, Type _nameC4)
	{
		bool result = ImGui::BeginTable("Data Table", 4, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollX | ImGuiTableFlags_Sortable);

		if (result)
		{
			ImGui::TableSetupColumn(_nameC1, ImGuiTableColumnFlags_NoClip);
			ImGui::TableSetupColumn(_nameC2, ImGuiTableColumnFlags_NoClip);
			ImGui::TableSetupColumn(_nameC3, ImGuiTableColumnFlags_NoClip);
			ImGui::TableSetupColumn(_nameC4, ImGuiTableColumnFlags_NoClip);

			ImGui::TableHeadersRow();
		}

		return result;
	}

	template<typename TypeC1, typename TypeC2, typename TypeC3, typename TypeC4>
	void Table4C::Entry(TypeC1 _valueC1, TypeC2 _valueC2, TypeC3 _valueC3, TypeC4 _valueC4)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		if constexpr (IsStringRelated<TypeC1>())
		{
			if constexpr (IsSameTypeCV<TypeC1, RoCStr>())
			{
				ImGui::Text(_valueC1);
			}
			else if constexpr (IsSameTypeCV<TypeC1, String>())
			{
				ImGui::Text(_valueC1.c_str());
			}
			else if constexpr (IsSameTypeCV<TypeC1, StringView>())
			{
				ImGui::Text(_valueC1.data());
			}
		}

		if constexpr (IsEnum<TypeC1>())
		{
			ImGui::Text(nameOf(_valueC1).data());
		}

		if constexpr (IsArithmetic<TypeC1>())
		{
			ImGui::Text(ToString(_valueC1).c_str());
		}

		ImGui::TableNextColumn();

		if constexpr (IsStringRelated<TypeC2>())
		{
			if constexpr (IsSameTypeCV<TypeC2, RoCStr>())
			{
				ImGui::Text(_valueC2);
			}
			else if constexpr (IsSameTypeCV<TypeC2, String>())
			{
				ImGui::Text(_valueC2.c_str());
			}
			else if constexpr (IsSameTypeCV<TypeC2, StringView>())
			{
				ImGui::Text(_valueC2.data());
			}
		}

		if constexpr (IsEnum<TypeC2>())
		{
			ImGui::Text(nameOf(_valueC2).data());
		}

		if constexpr (IsArithmetic<TypeC2>())
		{
			ImGui::Text(ToString(_valueC2).c_str());
		}

		ImGui::TableNextColumn();

		if constexpr (IsStringRelated<TypeC2>())
		{
			if constexpr (IsSameTypeCV<TypeC3, RoCStr>())
			{
				ImGui::Text(_valueC3);
			}
			else if constexpr (IsSameTypeCV<TypeC3, String>())
			{
				ImGui::Text(_valueC3.c_str());
			}
			else if constexpr (IsSameTypeCV<TypeC3, StringView>())
			{
				ImGui::Text(_valueC3.data());
			}
		}

		if constexpr (IsEnum<TypeC3>())
		{
			ImGui::Text(nameOf(_valueC3).data());
		}

		if constexpr (IsArithmetic<TypeC3>())
		{
			ImGui::Text(ToString(_valueC3).c_str());
		}

		ImGui::TableNextColumn();

		if constexpr (IsStringRelated<TypeC4>())
		{
			if constexpr (IsSameTypeCV<TypeC4, RoCStr>())
			{
				ImGui::Text(_valueC4);
			}
			else if constexpr (IsSameTypeCV<TypeC4, String>())
			{
				ImGui::Text(_valueC4.c_str());
			}
			else if constexpr (IsSameTypeCV<TypeC4, StringView>())
			{
				ImGui::Text(_valueC4.data());
			}
		}

		if constexpr (IsEnum<TypeC4>())
		{
			ImGui::Text(nameOf(_valueC4).data());
		}

		if constexpr (IsArithmetic<TypeC4>())
		{
			ImGui::Text(ToString(_valueC4).c_str());
		}
	}

	template<typename Type>
	bool Table5C::Record(Type _nameC1, Type _nameC2, Type _nameC3, Type _nameC4, Type _nameC5)
	{
		bool result = ImGui::BeginTable("Data Table", 5, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollX | ImGuiTableFlags_Sortable);

		if (result)
		{
			ImGui::TableSetupColumn(_nameC1);
			ImGui::TableSetupColumn(_nameC2);
			ImGui::TableSetupColumn(_nameC3);
			ImGui::TableSetupColumn(_nameC4);
			ImGui::TableSetupColumn(_nameC5);

			ImGui::TableHeadersRow();
		}

		return result;
	}

	template<typename TypeC1, typename TypeC2, typename TypeC3, typename TypeC4, typename TypeC5>
	void Table5C::Entry(TypeC1 _valueC1, TypeC2 _valueC2, TypeC3 _valueC3, TypeC4 _valueC4, TypeC5 _valueC5)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		if constexpr (IsStringRelated<TypeC1>())
		{
			if constexpr (IsSameTypeCV<TypeC1, RoCStr>())
			{
				ImGui::Text(_valueC1);
			}
			else if constexpr (IsSameTypeCV<TypeC1, String>())
			{
				ImGui::Text(_valueC1.c_str());
			}
			else if constexpr (IsSameTypeCV<TypeC1, StringView>())
			{
				ImGui::Text(_valueC1.data());
			}
		}

		if constexpr (IsEnum<TypeC1>())
		{
			ImGui::Text(nameOf(_valueC1).data());
		}

		if constexpr (IsArithmetic<TypeC1>())
		{
			ImGui::Text(ToString(_valueC1).c_str());
		}

		ImGui::TableNextColumn();

		if constexpr (IsStringRelated<TypeC2>())
		{
			if constexpr (IsSameTypeCV<TypeC2, RoCStr>())
			{
				ImGui::Text(_valueC2);
			}
			else if constexpr (IsSameTypeCV<TypeC2, String>())
			{
				ImGui::Text(_valueC2.c_str());
			}
			else if constexpr (IsSameTypeCV<TypeC2, StringView>())
			{
				ImGui::Text(_valueC2.data());
			}
		}

		if constexpr (IsEnum<TypeC2>())
		{
			ImGui::Text(nameOf(_valueC2).data());
		}

		if constexpr (IsArithmetic<TypeC2>())
		{
			ImGui::Text(ToString(_valueC2).c_str());
		}

		ImGui::TableNextColumn();

		if constexpr (IsStringRelated<TypeC2>())
		{
			if constexpr (IsSameTypeCV<TypeC3, RoCStr>())
			{
				ImGui::Text(_valueC3);
			}
			else if constexpr (IsSameTypeCV<TypeC3, String>())
			{
				ImGui::Text(_valueC3.c_str());
			}
			else if constexpr (IsSameTypeCV<TypeC3, StringView>())
			{
				ImGui::Text(_valueC3.data());
			}
		}

		if constexpr (IsEnum<TypeC3>())
		{
			ImGui::Text(nameOf(_valueC3).data());
		}

		if constexpr (IsArithmetic<TypeC3>())
		{
			ImGui::Text(ToString(_valueC3).c_str());
		}

		ImGui::TableNextColumn();

		if constexpr (IsStringRelated<TypeC4>())
		{
			if constexpr (IsSameTypeCV<TypeC4, RoCStr>())
			{
				ImGui::Text(_valueC4);
			}
			else if constexpr (IsSameTypeCV<TypeC4, String>())
			{
				ImGui::Text(_valueC4.c_str());
			}
			else if constexpr (IsSameTypeCV<TypeC4, StringView>())
			{
				ImGui::Text(_valueC4.data());
			}
		}

		if constexpr (IsEnum<TypeC4>())
		{
			ImGui::Text(nameOf(_valueC4).data());
		}

		if constexpr (IsArithmetic<TypeC4>())
		{
			ImGui::Text(ToString(_valueC4).c_str());
		}

		ImGui::TableNextColumn();

		if constexpr (IsStringRelated<TypeC5>())
		{
			if constexpr (IsSameTypeCV<TypeC5, RoCStr>())
			{
				ImGui::Text(_valueC5);
			}
			else if constexpr (IsSameTypeCV<TypeC5, String>())
			{
				ImGui::Text(_valueC5.c_str());
			}
			else if constexpr (IsSameTypeCV<TypeC5, StringView>())
			{
				ImGui::Text(_valueC5.data());
			}
		}

		if constexpr (IsEnum<TypeC5>())
		{
			ImGui::Text(nameOf(_valueC5).data());
		}

		if constexpr (IsArithmetic<TypeC5>())
		{
			ImGui::Text(ToString(_valueC5).c_str());
		}
	}
}