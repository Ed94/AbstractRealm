/*

*/



#pragma once



#include "Simulation/Units.hpp"



namespace Sim
{
	constexpr
	DecN CompareEpsilon()
	{
		return 
			
			LAL::IsOfClass<LAL::float32, DecN>() ? 

			Meta::Sim_DecN_UseHighAccuracy_Compare() ? LAL::Float32_HighAccuracy : LAL::Float32_LowAccuracy :
			(
				Meta::Sim_DecN_UseHighAccuracy_Compare() ?
				Meta::Sim_DecN_UseNanoAccuracy_Compare() ? LAL::Float64_NanoAccuracy : LAL::Float64_HighAccuracy :
				                                           LAL::Float64_LowAccuracy
			);
	}

	constexpr
	DecN CompareEpsilon_Squared()
	{
		return CompareEpsilon() * CompareEpsilon();
	}


	/*

	*/
	constexpr 
	DecN UnitEpsilon()
	{
		return 
			
			LAL::IsOfClass<LAL::float32, DecN>() ? 
			Meta::Sim_DecN_UseHighAccuracy_Unit() ? LAL::Float32_HighAccuracy : LAL::Float32_LowAccuracy : 
			(
				Meta::Sim_DecN_UseHighAccuracy_Unit() ? 
				Meta::Sim_DecN_UseNanoAccuracy_Unit() ? LAL::Float64_NanoAccuracy : LAL::Float64_HighAccuracy : 
				                                        LAL::Float64_LowAccuracy
			);
	}

	constexpr 
	DecN Sqrt_OneHalf()
	{
		return std::sqrt(DecN(1.0) / DecN(2.0));
	}
	
	constexpr
	DecN Sqrt_2()
	{
		return std::sqrt( DecN(2.0) );
	}

	constexpr
	DecN Log_2()
	{
		return std::log( DecN(2.0) );
	}

	constexpr
	DecN Pi()
	{
		return std::atan( DecN(1.0) ) * 4;
	}

	constexpr
	DecN Tau()
	{
		return std::atan( DecN(1.0) ) * 8;
	}

	constexpr
	DecN Euluer()
	{
		return std::exp( DecN(1.0) );
	}

	constexpr 
	DecN Infinity()
	{
		return LAL::IsOfClass<LAL::float32, DecN>() ? LAL::Float32_Infinity : LAL::Float64_Infinity;
	}

	constexpr 
	DecN NaN()
	{
		return LAL::IsOfClass<LAL::float32, DecN>() ? LAL::Float32_QNaN : LAL::Float64_QNaN;
	}
}
