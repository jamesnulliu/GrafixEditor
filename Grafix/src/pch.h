#pragma once

#ifdef GF_WINDOWS
#    include <Windows.h>
#endif // GF_WINDOWS

// ---- C++ Standard Library ----

#include <memory>
#include <ranges>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// ---- Dependencies ----

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// ---- Grafix ----

#include "Grafix/Core/Core.h"