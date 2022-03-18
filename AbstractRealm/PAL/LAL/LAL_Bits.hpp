/*
Bitfield

A wrapper for bitfields with support for specifying enum classes as bitmaskable types, and 
ease of use functionality for manipulating the bitfield.
*/


#pragma once


#include "LAL_Casting.hpp"
#include "LAL_Declarations.hpp"
#include "LAL_Reflection.hpp"
#include "LAL_Types.hpp"


namespace LAL
{
	template<uDM NumberOfBits>
	using Bitset = STL::bitset<NumberOfBits>;

#pragma region BitmaskableTraits

	template<typename Enum, typename = void>
	/**
	@brief Used when the enum does not meet the criteria for bitmaskable.
	*/
	struct IsBitmaskable : FalseType
	{};

	template<typename Enum>
	/**
	@brief Will be defined with a true_type when enum has the SpecifyBitmaskable enum value.
	*/
	struct IsBitmaskable<Enum, HasMemberSymbol(Enum::SpecifyBitmaskable)> : IsEnumType<Enum>
	{};

	template <typename Enum>
	/**
	@brief Returns true if IsBitmaskable is false.
	*/
	constexpr Where<IsBitmaskable<Enum>::value, 
	bool> Bitmaskable() noexcept
	{
		// Make sure enum member is greater than 0 (as it was supposed to be one of the last if not last entry).
		return SCast<uDM>(Enum::SpecifyBitmaskable) > uDM(0) ? true : false;
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

#pragma endregion BitmaskableTraits
	
	template
	<
		typename EnumType             ,
		typename BitmaskRepresentation
	>
	/**
	A wrapper object for bitfields that allows for typesafe bitmask operations.
	*/
	class Bitfield
	{
	private:
		EnforceConstraint(Bitmaskable<EnumType>(), "EnumType must be of Bitmaskable type.");

		using _ThisType = Bitfield<EnumType, BitmaskRepresentation>;

	public:

		using Enum           = EnumType             ;
		using Representation = BitmaskRepresentation;

		static constexpr uDM NumBits = sizeof(Representation) * 8;

		Bitfield() : bitfield(0) {}

		Bitfield(Representation _mask) : bitfield(_mask)
		{}

		template<typename... BitTypes>
		constexpr
		Bitfield(const BitTypes... _bits) : bitfield(0)
		{
			bitfield = (Representation(_bits) | ...);
		}

		template<typename... BitType>
		ForceInline
		void Add(const BitType... _bits)
		{
			bitfield |= (Representation(_bits) | ...);
		}

		template<typename... BitType>
		ForceInline
		bool CheckForEither(const BitType... _bits) const
		{
			return (bitfield & (Representation(_bits) | ...)) != 0;
		}

		template<typename... BitType>
		ForceInline
		void Clear(const BitType... _bits)
		{
			if (bitfield <= 0) 
				return;

			bitfield &= ~(Representation(_bits) | ...);
		}

		ForceInline
		bool HasFlag(const Enum _bit) const
		{
			return (bitfield & Representation(_bit)) == Representation(_bit);
		}

		template<typename... BitType>
		ForceInline
		bool HasExactly(const BitType... _bits) const
		{
			return (bitfield & (Representation(_bits) | ...)) == bitfield;
		}

		ForceInline
		bool HasAnyFlag() const 
		{
			return bitfield != 0 ? true : false; 
		}

		ForceInline
		bool IsZero() const 
		{
			return bitfield == 0 ? true : false; 
		}	

		ForceInline
		void Reset() 
		{
			bitfield = 0; 
		}

		template<typename... BitType>
		ForceInline
		void Set(const BitType... _bits)
		{
			bitfield = (Representation(_bits) | ...);
		}

		template<typename... BitType>
		ForceInline
		void Toggle(const BitType... _bits)
		{
			bitfield ^= (Representation(_bits) | ...);
		}

		operator Representation() const 
		{
			return bitfield; 
		}
		
		operator Bitset<NumBits>()
		{
			return bitfield;
		}

		String ToString()
		{
			std::stringstream stream;

			stream << Bitset<NumBits>(bitfield);

			return stream.str();
		}

		_ThisType& operator= (const Representation _mask ) { bitfield = _mask; return *this; }
		_ThisType& operator= (const _ThisType      _other) { bitfield = _other.bitfield; return *this; }

		_ThisType& operator&= (const Representation _mask ) { bitfield &= _mask; return *this; }
		_ThisType& operator&= (const _ThisType      _other) { bitfield &= _other.bitfield; return *this; }

		_ThisType& operator|= (const Representation _mask ) { bitfield |= _mask; return *this; }
		_ThisType& operator|= (const _ThisType      _other) { bitfield |= _other.bitfield; return *this; }	

		_ThisType& operator^= (const Representation _mask ) { bitfield ^= _mask; return *this; }
		_ThisType& operator^= (const _ThisType      _other) { bitfield ^= _other.bitfield; return *this; }	

		_ThisType& operator<<= (const Representation _mask ) { bitfield <<= _mask; return *this; }
		_ThisType& operator>>= (const _ThisType      _other) { bitfield >>= _other.bitfield; return *this; }	

		_ThisType operator~ () const { return ~bitfield; }

		Representation operator& (const Representation _other) const { return bitfield & _other; }
		_ThisType      operator& (const _ThisType      _other) const { return bitfield & _other.bitfield; }

		Representation operator| (const Representation _other) const { return bitfield | _other; }
		_ThisType      operator| (const _ThisType      _other) const { return bitfield | _other.bitfield; }

		Representation operator^ (const Representation _other) const { return bitfield ^ _other; }
		_ThisType      operator^ (const _ThisType      _other) const { return bitfield ^ _other.bitfield; }

		Representation operator<< (const Representation _other) const { return bitfield << _other; }
		_ThisType      operator>> (const _ThisType      _other) const { return bitfield >> _other.bitfield; }

		bool operator== (const Representation _other) const { return bitfield == _other; }
		bool operator== (const _ThisType      _other) const { return bitfield == _other.bitfield; }

		bool operator!= (const Representation _other) const { return bitfield != _other; }
		bool operator!= (const _ThisType      _other) const { return bitfield != _other.bitfield; }

	private:

		Representation bitfield;
	};
}
