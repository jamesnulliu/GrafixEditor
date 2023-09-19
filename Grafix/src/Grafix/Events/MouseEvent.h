#pragma once

#include "Event.h"
#include "Grafix/Input/Codes.h"

namespace Grafix
{
    class MouseMovedEvent final : public Event
    {
    public:
        MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}

        inline float GetX() const { return m_MouseX; }
        inline float GetY() const { return m_MouseY; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: (" << m_MouseX << ", " << m_MouseY << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)
            EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    protected:
        float m_MouseX, m_MouseY;
    };

    class MouseScrolledEvent final : public Event
    {
    public:
        MouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {}

        inline float GetXOffset() const { return m_XOffset; }
        inline float GetYOffset() const { return m_YOffset; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseScrolledEvent: (" << m_XOffset << ", " << m_YOffset << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseScrolled)
            EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    protected:
        float m_XOffset, m_YOffset;
    };

    class MouseButtonEvent : public Event
    {
    public:
        inline MouseButton GetMouseButton() const { return m_Button; }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    protected:
        MouseButtonEvent(MouseButton button) : m_Button(button) {}

        MouseButton m_Button;
    };

    class MouseButtonPressedEvent final : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent(MouseButton button) : MouseButtonEvent(button) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << (uint16_t)m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent final : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent(MouseButton button) : MouseButtonEvent(button) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << (uint16_t)m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };
}
