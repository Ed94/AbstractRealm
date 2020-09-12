/*
Bitmask

A wrapper for bitmasks with support for specifying enum classes as bitmaskable types, and 
ease of use functionality for manipulating the mask.
*/



#pragma once



#include "LAL_Casting.hpp"
#include "LAL_Declarations.hpp"
#include "LAL_Reflection.hpp"
#include "LAL_Types.hpp"



#ifndef BITMASK_DEFINED
#define BITMASK_DEFINED

	/*template<typename Enum>
	struct Bitmaskable
	{
		static constexpr bool specified = false;
	};

	#define SpecifyBitmaskable(__ENUM)          \
	template<>                                  \
	struct Bitmaskable<__ENUM>                  \
	{                                           \
		static constexpr bool specified = true; \
	};*/

#endif



namespace LAL
{
	template<typename Enum, typename = void>
	struct IsBitmaskable : std::false_type
	{};

	template<typename Enum>
	struct IsBitmaskable<Enum, decltype(SCast<void>(Enum::SpecifyBitmaskable))> : IsEnumType<Enum>
	{};

	template <typename Enum>
	constexpr typename Where<IsBitmaskable<Enum>::value, 
	bool> Bitmaskable() noexcept
	{
		return static_cast<WordSize>(Enum::SpecifyBitmaskable) > WordSize(0) ? true : false;
	}

	template <typename Enum> 
	constexpr typename Where<!IsBitmaskable<Enum>::value, 
	bool> Bitmaskable() noexcept
	{
		return false;
	}

	/*template<typename Enum>
	using Bitmaskable = TBitmaskable<Enum>;*/

	template
	<
		typename EnumType,
		typename BitmaskRepresentation
	>
	struct Bitmask
	{
	private:
		EnforceConstraint(Bitmaskable<EnumType>(), "EnumType must be of Bitmaskable type.");

		using _ThisType = Bitmask<EnumType, BitmaskRepresentation>;

	public:

		using Representation = BitmaskRepresentation;
		using Enum           = EnumType             ;

		Bitmask() : mask(0) {}

		template<typename... BitTypes>
		Bitmask(const BitTypes... _bits) : mask(0)
		{
			mask = (Representation(_bits) | ...);
		}

		template<typename... BitType>
		void Add(const BitType... _bits)
		{
			mask |= (Representation(_bits) | ...);
		}

		void Clear()
		{
			mask = 0;
		}

		bool Has(const Enum _bit) const
		{
			return (mask & Representation(_bit)) == Representation(_bit);
		}

		template<typename... BitType>
		bool HasOrEither(const BitType... _bits) const
		{
			return (mask & (Representation(_bits) | ...)) != 0;
		}

		template<typename... BitType>
		bool HasExactly(const BitType... _bits) const
		{
			return (mask & (Representation(_bits) | ...)) == mask;
		}

		template<typename... BitType>
		void Remove(const BitType... _bits)
		{
			if (mask <= 0) return;

			mask &= ~(Representation(_bits) | ...);
		}

		template<typename... BitType>
		void Set(const BitType... _bits)
		{
			mask = (Representation(_bits) | ...);
		}

		_ThisType& operator= (const BitmaskRepresentation _mask) { mask = _mask; return *this; }

		operator Representation() const
		{
			return mask;
		}

		bool operator== (const Representation _other) const
		{
			return mask == _other;
		}

		bool operator== (const _ThisType& _other) const
		{
			return mask == _other.mask;
		}

	private:
		Representation mask;
	};
}
