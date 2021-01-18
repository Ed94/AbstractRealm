/*
Bitfield

A wrapper for bitmasks with support for specifying enum classes as bitmaskable types, and 
ease of use functionality for manipulating the bitfield.
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
	/**
	@brief Used when the enum does not meet the criteria for bitmaskable.
	*/
	struct IsBitmaskable : std::false_type
	{};

	template<typename Enum>
	/**
	@brief Will be defined with a true_type when enum has the SpecifyBitmaskable enum value.
	*/
	struct IsBitmaskable<Enum, decltype(static_cast<void>(Enum::SpecifyBitmaskable))> : IsEnumType<Enum>
	{};

	template <typename Enum>
	/**
	@brief Returns true if IsBitmaskable is false.
	*/
	constexpr Where<IsBitmaskable<Enum>::value, 
	bool> Bitmaskable() noexcept
	{
		return static_cast<WordSize>(Enum::SpecifyBitmaskable) > WordSize(0) ? true : false;
	}

	template <typename Enum> 
	/**
	@brief Returns false if bitmaskable is false (Default case).
	*/
	constexpr Where<! IsBitmaskable<Enum>::value, 
	bool> Bitmaskable() noexcept
	{
		return false;
	}

	template
	<
		typename EnumType             ,
		typename BitmaskRepresentation
	>
	struct Bitfield
	{
	private:
		EnforceConstraint(Bitmaskable<EnumType>(), "EnumType must be of Bitmaskable type.");

		using _ThisType = Bitfield<EnumType, BitmaskRepresentation>;

	public:

		using Enum           = EnumType             ;
		using Representation = BitmaskRepresentation;

		Bitfield() : mask(0) {}

		Bitfield(Representation _mask) : mask(_mask)
		{}

		template<typename... BitTypes>
		Bitfield(const BitTypes... _bits) : mask(0)
		{
			mask = (Representation(_bits) | ...);
		}

		template<typename... BitType>
		void Add(const BitType... _bits)
		{
			mask |= (Representation(_bits) | ...);
		}

		template<typename... BitType>
		bool CheckForEither(const BitType... _bits) const
		{
			return (mask & (Representation(_bits) | ...)) != 0;
		}

		template<typename... BitType>
		void Clear(const BitType... _bits)
		{
			if (mask <= 0) return;

			mask &= ~(Representation(_bits) | ...);
		}

		bool HasFlag(const Enum _bit) const
		{
			return (mask & Representation(_bit)) == Representation(_bit);
		}

		template<typename... BitType>
		bool HasExactly(const BitType... _bits) const
		{
			return (mask & (Representation(_bits) | ...)) == mask;
		}

		bool HasAnyFlag() const { return mask != 0 ? true : false; }
		bool IsZero    () const { return mask == 0 ? true : false; }	

		void Reset() { mask = 0; }

		template<typename... BitType>
		void Set(const BitType... _bits)
		{
			mask = (Representation(_bits) | ...);
		}

		template<typename... BitType>
		void Toggle(const BitType... _bits)
		{
			mask ^= (Representation(_bits) | ...);
		}

		operator Representation() const { return mask; }

		_ThisType& operator= (const Representation _mask ) { mask = _mask      ; return *this; }
		_ThisType& operator= (const _ThisType      _other) { mask = _other.mask; return *this; }

		_ThisType& operator&= (const Representation _mask ) { mask &= mask       ; return *this; }
		_ThisType& operator&= (const _ThisType      _other) { mask &= _other.mask; return *this; }

		_ThisType& operator|= (const Representation _mask ) { mask |= mask       ; return *this; }
		_ThisType& operator|= (const _ThisType      _other) { mask |= _other.mask; return *this; }	

		_ThisType& operator^= (const Representation _mask ) { mask ^= mask       ; return *this; }
		_ThisType& operator^= (const _ThisType      _other) { mask ^= _other.mask; return *this; }	

		_ThisType& operator<<= (const Representation _mask ) { mask <<= mask       ; return *this; }
		_ThisType& operator>>= (const _ThisType      _other) { mask >>= _other.mask; return *this; }	

		_ThisType operator~ () const { return ~mask; }

		Representation operator& (const Representation _other) const { return mask & _other     ; }
		_ThisType      operator& (const _ThisType      _other) const { return mask & _other.mask; }

		Representation operator| (const Representation _other) const { return mask | _other     ; }
		_ThisType      operator| (const _ThisType      _other) const { return mask | _other.mask; }

		Representation operator^ (const Representation _other) const { return mask ^ _other     ; }
		_ThisType      operator^ (const _ThisType      _other) const { return mask ^ _other.mask; }

		Representation operator<< (const Representation _other) const { return mask << _other     ; }
		_ThisType      operator>> (const _ThisType      _other) const { return mask >> _other.mask; }

		bool operator== (const Representation _other) const { return mask == _other     ; }
		bool operator== (const _ThisType      _other) const { return mask == _other.mask; }

		bool operator!= (const Representation _other) const { return mask != _other     ; }
		bool operator!= (const _ThisType      _other) const { return mask != _other.mask; }

	private:

		Representation mask;
	};
}
