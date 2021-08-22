#pragma once


#include "Objects_Backend.hpp"


namespace Core
{
#pragma region ObjectSignatures
	class Object;

	template<typename TypeDef> 
	class TObject;
#pragma region ObjectSignatures


	class VPtr
	{
	public:

	#pragma region Constructors
		VPtr()
		: 
			rPtr(nullptr)
		{}

		template<typename Type>
		VPtr(Type& _object)
		:
			rPtr(getPtr(_object))
		{}

		VPtr(ptr<void> _ptr)
		:
			rPtr(_ptr)
		{}
	#pragma endregion Constructors

	#pragma region Implicit Casts

		operator ptr<void>()
		{
			return rPtr;
		}

	#pragma endregion Implicit Casts

	protected:

		ptr<void> rPtr;
	};


	template<typename TypeDef>
	class TPtr
	{
	public:

		using Type = TypeDef;

	#pragma region Constructors
		TPtr()
		:
			rPtr(nullptr)
		{}

		TPtr(Type& _object)
		:	
			rPtr(getPtr(_object))
		{}

		TPtr(ptr<Type> _ptr)
		:
			rPtr(_ptr)
		{}
	#pragma endregion Constructors

		ForceInline
		bool IsValid()
		{
			return rPtr == nullptr;
		}

		Type& Obj()
		{
			if (rPtr == nullptr)
				Exception::Fatal::NullReference("ObjPtr is null!");

			return dref(rPtr);
		}

	#pragma region Implicit Casts

		explicit operator ptr<void>()
		{
			return rPtr;
		}

	#pragma endregion Implicit Casts

	protected:

		ptr<Type> rPtr;
	};


	class ObjPtr : TPtr<Object>
	{
	public:

	#pragma region Constructors
		ObjPtr(Object& _obj) : TPtr(_obj)
		{}

		ObjPtr(ptr<Object> _ptr) : TPtr(_ptr)
		{}
	#pragma endregion Constructors

	protected:
	};
}

