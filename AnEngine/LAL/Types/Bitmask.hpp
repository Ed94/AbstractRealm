#pragma once


#define Bitmaskable \
enum class


namespace LAL
{
	template<typename EnumType, typename BitmaskRepresentation>
	struct bitmask
	{
	private:
		using _self = bitmask<EnumType, BitmaskRepresentation>;

	public:

		using Representation = BitmaskRepresentation;
		using Enum           = EnumType             ;

		template<typename... BitType>
		void Add(const BitType... _bits)
		{
			mask |= (Representation(_bits) | ...);
		}

		bool Has(const Enum _bit) const
		{
			return mask & Representation(_bit);
		}

		template<typename... BitType>
		void Set(const BitType... _bits)
		{
			mask = (Representation(_bits) | ...);
		}

		_self& operator = (const BitmaskRepresentation& _mask ) { mask = _mask; }

		operator Representation()
		{
			return mask;
		}

	private:
		Representation mask;
	};
}



