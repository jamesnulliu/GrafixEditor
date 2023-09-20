#pragma once

#include "Grafix/Scene/Components.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

namespace Grafix
{
    enum class EntityType
    {
        Pixel = 0,
        Line, Arc,
        Polygon
    };

    class Entity
    {
    public:
        Entity(const std::string& name) : m_Name(name) {}
        virtual ~Entity() = default;

        virtual const std::string& GetName() const { return m_Name; }
    protected:
        std::string m_Name;
    };
}