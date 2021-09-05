#pragma once

#include "HAL_Backend.hpp"


namespace HAL::GPU::Vulkan
{
#pragma region VulkanVertex
	template
	<
		typename Type,
		typename = void,   // Type::GetAttributeDescriptions
		typename = void    // Type::GetBindingDescriptions
	>
	// Fail case for IsVulkanVertex
	struct IsVulkanVertexType : FalseType
	{};

	template<typename Type>
	/**
	@brief Will be defined with a TrueType when Type has the function.
	*/
	struct IsVulkanVertexType
	<
		Type,
		HasMemberSymbol(Type::GetAttributeDescriptions),
		HasMemberSymbol(Type::GetBindingDescription)
	>
	: IsClassType<Type>
	{};

	template<typename Type>
	constexpr bool IsVulkanVertex()
	{
		return IsVulkanVertexType<Type>::value;
	}

	template<typename Type>
	/**
	@brief Returns true if IsVulkanVertex is true. Verifies that static member addresses are correct as well.
	*/
	constexpr Where<IsVulkanVertex<Type>(),
	bool> VulkanVertex() noexcept
	{
		return
			HasMemberAddress(Type::GetAttributeDescriptions) &&
			HasMemberAddress(Type::GetBindingDescription);
	}

	template<typename Type>
	// Fail case for VulkanVertex
	constexpr Where<!IsVulkanVertex<Type>(),
	bool> VulkanVertex() noexcept
	{
		return false;
	}
#pragma region VulkanVertex
}
