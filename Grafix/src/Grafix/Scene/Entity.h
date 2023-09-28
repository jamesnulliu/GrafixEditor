#pragma once

#include "Scene.h"
#include "Components.h"

#include <entt.hpp>

namespace Grafix
{
    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene) : m_Handle(handle), m_Scene(scene) {}
        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;
        ~Entity() = default;

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            GF_ASSERT(!HasComponent<T>(), "Entity already has this type of component!");

            return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
        }

        template<typename T>
        void RemoveComponent()
        {
            GF_ASSERT(HasComponent<T>(), "Entity does not have this type of component!");
            m_Scene->m_Registry.remove<T>(m_Handle);
        }

        template<typename... T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.all_of<T...>(m_Handle);
        }

        template<typename... T>
        decltype(auto) GetComponent()
        {
            GF_ASSERT(HasComponent<T...>(), "Entity does not have this/these component(s)!");
            return m_Scene->m_Registry.get<T...>(m_Handle);
        }

        const std::string& GetTag() { return GetComponent<TagComponent>().Tag; }

        operator entt::entity() const { return m_Handle; }
        operator bool() const { return m_Handle != entt::null; }

        bool operator==(const Entity& rhs) const { return m_Handle == rhs.m_Handle && m_Scene == rhs.m_Scene; }
        bool operator!=(const Entity& rhs) const { return !(*this == rhs); }
    protected:
        entt::entity m_Handle = entt::null;
        Scene* m_Scene = nullptr;
    };
}