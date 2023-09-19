#pragma once

#include "Event.h"

namespace Grafix
{
    class WindowResizeEvent final : public Event
    {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

        inline uint32_t GetWidth() const { return m_Width; }
        inline uint32_t GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: (" << m_Width << ", " << m_Height << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)
    private:
        uint32_t m_Width, m_Height;
    };

    class WindowCloseEvent final : public Event
    {
    public:
        EVENT_CLASS_TYPE(WindowClose)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppTickEvent final : public Event
    {
    public:
        EVENT_CLASS_TYPE(AppTick)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppUpdateEvent final : public Event
    {
    public:
        EVENT_CLASS_TYPE(AppUpdate)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppRenderEvent final : public Event
    {
    public:
        EVENT_CLASS_TYPE(AppRender)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
}
