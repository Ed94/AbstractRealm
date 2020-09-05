/*
Bitmask

A wrapper for bitmasks with support for specifying enum classes as bitmaskable types, and 
ease of use functionality for manipulating the mask.
*/



#pragma once



#include "LAL_Declarations.hpp"



#ifndef BITMASK_DEFINED
#define BITMASK_DEFINED

	template<typename Enum>
	struct Bitmaskable
	{
		static constexpr bool specified = false;
	};

	#define SpecifyBitmaskable(__ENUM)          \
	template<>                                  \
	struct Bitmaskable<__ENUM>                  \
	{                                           \
		static constexpr bool specified = true; \
	};

#endif



namespace LAL
{
	template
	<
		typename EnumType,
		typename BitmaskRepresentation
	>
	struct Bitmask
	{
	private:
		EnforceConstraint(Bitmaskable<EnumType>::specified, "EnumType must be of Bitmaskable type.");

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
