#pragma once

#ifndef HPP_PrecompiledHeader
#pragma region Thirdparty APIs
#include "API.WIndows.hpp"
#pragma endregion Thirdparty APIs

#pragma region LAL
#include "LAL.Asserts.hpp"
#include "LAL.Bit.hpp"
#include "LAL.Casting.hpp"
#include "LAL.Config.hpp"
#include "LAL.Declarations.hpp"
#include "LAL.Math.hpp"
#include "LAL.Memory.hpp"
#include "LAL.Memory.Allocators.hpp"
#include "LAL.Platform.hpp"
#include "LAL.Reflection.hpp"
#include "LAL.Types.hpp"
#pragma endregion LAL

#pragma region OSAL
#include "OSAL.Platform.hpp"

#pragma endregion OSAL
#endif

#ifdef HPP_PrecompiledHeader
static_assert(false, "Precompiled header used more than once in a translation unit...");
#endif
#define HPP_PrecompiledHeader

