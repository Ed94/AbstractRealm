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
		VPtr(Type& _object_in)
		:
			rPtr(getPtr(_object_in))
		{}

		VPtr(ptr<void> _ptr_in)
		:
			rPtr(_ptr_in)
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

		TPtr(Type& _object_in)
		:	
			rPtr(getPtr(_object_in))
		{}

		TPtr(ptr<Type> _ptr_in)
		:
			rPtr(_ptr_in)
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
		ObjPtr(Object& _obj_in) : TPtr(_obj_in)
		{}

		ObjPtr(ptr<Object> _ptr_in) : TPtr(_ptr_in)
		{}
	#pragma endregion Constructors

	protected:
	};
}

