/*
C++ Standard Library Interface
*/


#pragma once

#include "Meta/Config/LAL_Config.hpp"


#ifdef META_USE_STANDARD_LIBRARY

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Weverything"
#endif
	#include <algorithm>
	#include <array>
	#include <bitset>
	#include <chrono>
	#include <filesystem>
	#include <fstream>
	#include <functional>
	#include <iosfwd>
	#include <iostream>
	#include <limits>
	#include <list>
	#include <iterator>
	#include <map>
	#include <memory>
	#include <mutex>
	#include <optional>
	#include <queue>
	#include <optional>
	#include <ratio>
	#include <stdexcept>
	#include <set>
	#include <string>
	#include <string_view>
	#include <sstream> 
	#include <thread>
	#include <typeinfo>
	#include <type_traits>
	#include <unordered_map>
	#include <unordered_set>
	#include <utility>
	#include <vector>
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

	namespace STL
	{
		using namespace std;
	}
#endif

#ifdef META_USE_EASTL

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Weverything"
#endif
	#include <filesystem>
	#include <filesystem>
	#include <fstream>

	#include "EASTL/algorithm.h"
	#include "EASTL/allocator.h"
	#include "EASTL/allocator_malloc.h"
	#include "EASTL/any.h"
	#include "EASTL/array.h"
	#include "EASTL/atomic.h"
	#include "EASTL/bitset.h"
	#include "EASTL/bitvector.h"
	#include "EASTL/chrono.h"
	#include "EASTL/core_allocator.h"
	#include "EASTL/core_allocator_adapter.h"
	#include "EASTL/deque.h"
	#include "EASTL/finally.h"
	#include "EASTL/fixed_allocator.h"
	#include "EASTL/fixed_function.h"
	#include "EASTL/fixed_hash_map.h"
	#include "EASTL/fixed_hash_set.h"
	#include "EASTL/fixed_list.h"
	#include "EASTL/fixed_map.h"
	#include "EASTL/fixed_set.h"
	#include "EASTL/fixed_slist.h"
	#include "EASTL/fixed_string.h"
	#include "EASTL/fixed_substring.h"
	#include "EASTL/fixed_vector.h"
	#include "EASTL/functional.h"
	#include "EASTL/hash_map.h"
	#include "EASTL/hash_set.h"
	#include "EASTL/heap.h"
	#include "EASTL/initializer_list.h"
	#include "EASTL/intrusive_hash_map.h"
	#include "EASTL/intrusive_hash_set.h"
	#include "EASTL/intrusive_list.h"
	#include "EASTL/instrusive_ptr.h"
	#include "EASTL/iterator.h"
	#include "EASTL/linked_array.h"
	#include "EASTL/linked_ptr.h"
	#include "EASTL/list.h"
	#include "EASTL/map.h"
	#include "EASTL/memory.h"
	#include "EASTL/meta.h"
	#include "EASTL/numeric.h"
	#include "EASTL/numeric_limits.h"
	#include "EASTL/optional.h"
	#include "EASTL/priority_queue.h"
	#include "EASTL/random.h"
	#include "EASTL/ratio.h"
	#include "EASTL/safe_ptr.h"
	#include "EASTL/scoped_ptr.h"
	#include "EASTL/segmented_vector.h"
	#include "EASTL/set.h"
	#include "EASTL/shared_array.h"
	#include "EASTL/shared_ptr.h"
	#include "EASTL/slist.h"
	#include "EASTL/sort.h"
	#include "EASTL/span.h"
	#include "EASTL/stack.h"
	#include "EASTL/string.h"
	#include "EASTL/string_hash_map.h"
	#include "EASTL/string_map.h"
	#include "EASTL/string_view.h"
	#include "EASTL/tuple.h"
	#include "EASTL/type_traits.h"
	#include "EASTL/unique_ptr.h"
	#include "EASTL/unordered_map.h"
	#include "EASTL/unordered_set.h"
	#include "EASTL/utility.h"
	#include "EASTL/variant.h"
	#include "EASTL/vector.h"
	#include "EASTL/vector_map.h"
	#include "EASTL/vector_multimap.h"
	#include "EASTL/vector_multiset.h"
	#include "EASTL/vector_set.h"
	#include "EASTL/version.h"
	#include "EASTL/weak_ptr.h"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

	namespace STL
	{
		using namespace eastl;
	}
#endif

#pragma region ThirdParty

#include "nameof.hpp"

#pragma endregion ThirdParty
