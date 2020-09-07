/*
Calling Convention Enforcer

A macro tool for defining Enforcers for a template calling convention.

Requires std::forward.

C++17
*/



#pragma once



#ifndef CALLING_CONVENTION_ENFORCER_H
#define CALLING_CONVENTION_ENFORCER_H


	template <class ID>
	struct CallEnforcementSet;

	#define MakeCallEnforcer_ConventionID(__API_NAME) \
	class CallEnforcerID_##__API_NAME;

	#define MakeCallEnforcer_EnforcementSet(__API_NAME, __ATTRIBUTE, __CALL)         \
	template<>                                                                       \
	struct CallEnforcementSet<CallEnforcerID_##__API_NAME>                           \
	{                                                                                \
		template<typename FunctionType, FunctionType*>                               \
		struct CallEnforcer_CallMaker;                                               \
		                                                                             \
		template                                                                     \
		<                                                                            \
			typename    ReturnType    ,                                              \
			typename... ParameterTypes,                                              \
			ReturnType(*FunctionType)(ParameterTypes...)                             \
		>                                                                            \
		struct CallEnforcer_CallMaker<ReturnType(ParameterTypes...), FunctionType>   \
		{                                                                            \
			static __ATTRIBUTE ReturnType __CALL Call(ParameterTypes... _parameters) \
			{                                                                        \
				return FunctionType(std::forward<ParameterTypes>(_parameters)...);   \
			}                                                                        \
		};                                                                           \
	};

	#define MakeCallEnforcer(__API_NAME, __ATTRIBUTE, __CALL)        \
	MakeCallEnforcer_ConventionID(__API_NAME);                       \
	MakeCallEnforcer_EnforcementSet(__API_NAME, __ATTRIBUTE, __CALL);

	template
	<
		class    ID          ,
		typename FunctionType,

		FunctionType& _functionRef
	>
	auto GetEnforcedCall()
	{
		return &(CallEnforcementSet<ID>::template CallEnforcer_CallMaker<FunctionType, &_functionRef>::Call);
	};

	/*
	Ease of use macro to call the Enforcer_Caller for the defined API convention.
	*/
	#define EnforceCallingConvention(__API_NAME, __FUNCTION)       \
	GetEnforcedCall<__API_NAME, decltype(__FUNCTION), __FUNCTION>()

#endif

