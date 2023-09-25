#pragma once

#include "Components.h"

#include <vector>

namespace Grafix
{
    class Entity
    {
    public:
        Entity() = default;
        virtual ~Entity() = default;

        template<typename T, typename... Args>
        T* AddComponent(Args&&... args)
        {
            GF_ASSERT(!GetComponent<T>(), "Entity already has this type of component!");

            T* component = new T(std::forward<Args>(args)...);
            m_Components.push_back(static_cast<Component*>(component));

            return component;
        }

        template<typename T>
        T* GetComponent()
        {
            for (auto component : m_Components)
            {
                if (dynamic_cast<T*>(component))
                    return dynamic_cast<T*>(component);
            }

            return nullptr;

            ////GF_ASSERT(false, "Entity does not have this type of component!");
        }

        ////template<ComponentType T>
        ////bool HasComponent()
        ////{
        ////    for (auto& component : m_Components)
        ////    {
        ////        if (dynamic_cast<T*>(component))
        ////            return true;
        ////    }
        ////    return false;
        ////}

        template<typename T>
        void RemoveComponent()
        {
            for (auto& component : m_Components)
            {
                if (dynamic_cast<T*>(component))
                {
                    delete component;
                    m_Components.erase(component);
                }
            }
        }

        bool operator==(const Entity& other) const
        {
            if (m_Components.size() != other.m_Components.size())
                return false;

            return true;
        }
    protected:
        std::vector<Component*> m_Components;
    };
}