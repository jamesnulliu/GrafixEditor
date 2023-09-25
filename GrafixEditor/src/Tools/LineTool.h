#pragma once

#include "Tool.h"

namespace Grafix
{
    class LineTool final : public Tool
    {
    public:
        LineTool();
        virtual ~LineTool() = default;
    private:
        void OnUpdate();
        bool m_IsDrawing = false;

        struct LineSpecification
        {
            glm::vec2 P0;
            glm::vec2 P1;
            float Thickness = 1.0f;
            glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        };
        LineSpecification m_LineSpec;
    };
}
