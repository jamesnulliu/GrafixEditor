#pragma once

#include "Log.h"

#ifdef GE_DEBUG
#    define GE_ENABLE_ASSERTS
#endif

#ifdef GE_ENABLE_ASSERTS
#    define GE_ASSERT(x, ...) { if(!(x)) { GE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else  // GE_ENABLE_ASSERTS
#    define GE_ASSERT(x, ...)
#endif  // GE_ENABLE_ASSERTS
