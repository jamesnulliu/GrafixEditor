#pragma once

#include "Log.h"

#ifdef GF_DEBUG
#    define GF_ENABLE_ASSERTS
#endif

#ifdef GF_ENABLE_ASSERTS
#    define GF_ASSERT(x, ...) { if(!(x)) { GF_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else  // GF_ENABLE_ASSERTS
#    define GF_ASSERT(x, ...)
#endif  // GF_ENABLE_ASSERTS

#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
