#pragma once

#ifndef HPP_PrecompiledHeader
#pragma region Thirdparty APIs
#include "API.WIndows.hpp"
#pragma endregion Thirdparty APIs

#pragma region LAL
#include "LAL.Asserts.hpp"
#include "LAL.Bits.hpp"
#include "LAL.Casting.hpp"
#include "LAL.CString.hpp"
#include "LAL.Config.hpp"
#include "LAL.DArray.hpp"
#include "LAL.Data.hpp"
#include "LAL.Declarations.hpp"
#include "LAL.Limits.hpp"
#include "LAL.Math.hpp"
#include "LAL.Memory.hpp"
#include "LAL.Memory.Allocators.hpp"
#include "LAL.Metaprogramming.hpp"
#include "LAL.NumericLimits.hpp"
#include "LAL.Numerics.hpp"
#include "LAL.Platform.hpp"
#include "LAL.RefWrap.hpp"
#include "LAL.String.hpp"
#include "LAL.Tuple.hpp"
#include "LAL.Types.hpp"
#include "LAL.UTF.hpp"
#pragma endregion LAL

#pragma region OSAL
#include "OSAL.Platform.hpp"

#pragma endregion OSAL
#endif

#ifdef HPP_PrecompiledHeader
static_assert(false, "Precompiled header used more than once in a translation unit...");
#endif
#define HPP_PrecompiledHeader

