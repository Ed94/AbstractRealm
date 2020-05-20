/*
Convention Enforcer

Last Modfied: 5/20/2020

A macro tool for defining Enforcers for a template calling convention.

MakeEnforce generates the template.
From there you need to make a macro for nicely calling the enforcer function.

Note: It was designed to be used within a namespace.

Requires std::forward.
*/



#pragma once



/*
Do not directly use... its part of the MakeEnforcementSet.

Creates the base template for the Enforced calling convention maker struct.
*/
#define MakeEnforcer_MakerTemplate_Base(__API_NAME) \
template<typename FunctionType, FunctionType* >     \
struct Enforcer_##__API_NAME##_CallMaker;

/*
Do not directly use... its part of the MakeEnforcementSet.

Creates the implementation template for the Enforced calling convention maker struct.
*/
#define MakeEnforcer_MakerTemplate_Implementation(__API_NAME, __ATTRIBUTE, __CALL)    \
template                                           \
<                                                                                     \
	typename    ReturnType    ,                                                       \
	typename... ParameterTypes,	                                                      \
	ReturnType(*FunctionType)(ParameterTypes...)                                      \
>												                                      \
struct Enforcer_##__API_NAME##_CallMaker<ReturnType(ParameterTypes...), FunctionType> \
{					                                                                  \
	static __ATTRIBUTE ReturnType __CALL Call(ParameterTypes... _parameters)          \
	{																		          \
		return FunctionType(std::forward<ParameterTypes>(_parameters)...);            \
	}																		          \
};

/*
/*
Do not directly use... its part of the MakeEnforcementSet.

Creates the caller function tempalte for the Enforced calling convention maker struct.
*/
#define MakeEnforcer_MakerCaller(__API_NAME)                                     \
template<typename FunctionType, FunctionType& _callback>                         \
auto Enforcer_##__API_NAME##_Caller()                                            \
{                                                                                \
	return &(Enforcer_##__API_NAME##_CallMaker<FunctionType, &_callback>::Call); \
};

/*
The enforcer generator. Produces all the templates to do generic calling convention enforcement
for the specified api's attribute and call calling conventions.

You can from here call the generated enforcer caller template, or use the
EnforceConvention() macro.
*/
#define MakeEnforcementSet(__API_NAME, __ATTRIBUTE, __CALL)                 \
MakeEnforcer_MakerTemplate_Base(__API_NAME);                                \
MakeEnforcer_MakerTemplate_Implementation(__API_NAME, __ATTRIBUTE, __CALL); \
MakeEnforcer_MakerCaller(__API_NAME);                                       

/*
Ease of use macro. Allows for auto calling of the enforcer caller template with the associated
api. (So that template arguments do not have to be manually specified)
*/
#define EnforceConvention(__API_NAME, _function) \
Enforcer_##__API_NAME##_Caller<decltype(_function), _function>();
