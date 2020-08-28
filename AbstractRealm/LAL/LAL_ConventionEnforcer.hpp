/*
Convention Enforcer

Last Modfied: 5/21/2020

A macro tool for defining Enforcers for a template calling convention.

Requires std::forward.

C++17
*/



#pragma once



#ifndef CONVENTION_ENFORCER_H

	#define CONVENTION_ENFORCER_H



	template <class ID>
	struct EnforcementSet;

	#define MakeConventionEnforcer_ConventionID(__API_NAME) \
	class EnforcerID_##__API_NAME;

	#define MakeConventionEnforcer_EnforcementSet(__API_NAME, __ATTRIBUTE, __CALL)   \
	template<>																	     \
	struct EnforcementSet<EnforcerID_##__API_NAME>					                 \
	{																			     \
		template<typename FunctionType, FunctionType*>                               \
		struct Enforcer_CallMaker;                                                   \
																					 \
		template																	 \
		<																			 \
		typename ReturnType,                                                         \
		typename... ParameterTypes,                                                  \
		ReturnType(*FunctionType)(ParameterTypes...)                                 \
		>											                                 \
		struct Enforcer_CallMaker<ReturnType(ParameterTypes...), FunctionType>       \
		{																		     \
			static __ATTRIBUTE ReturnType __CALL Call(ParameterTypes... _parameters) \
			{																		 \
				return FunctionType(std::forward<ParameterTypes>(_parameters)...);	 \
			}																		 \
		};																			 \
	};

	#define MakeConventionEnforcer(__API_NAME, __ATTRIBUTE, __CALL)        \
	MakeConventionEnforcer_ConventionID(__API_NAME);                       \
	MakeConventionEnforcer_EnforcementSet(__API_NAME, __ATTRIBUTE, __CALL);

	template
	<
		class ID,
		typename FunctionType,
		FunctionType& _functionRef
	>
	auto Enforcer_Caller()
	{
		return &(EnforcementSet<ID>::template Enforcer_CallMaker<FunctionType, &_functionRef>::Call);
	};

	#define EnforceConvention(__API_NAME, __FUNCTION)               \
	Enforcer_Caller<__API_NAME, decltype(__FUNCTION), __FUNCTION>();


#endif