#pragma once

#include "Tool.h"

namespace Grafix
{
    class LineTool final : public Tool
    {
    public:
        LineTool();
        virtual ~LineTool() = default;

        void Bind(LineRendererComponent* lineRenderer);

        void OnUpdate();

        void UnBind();

    private:
        bool m_IsDrawing = false;
    };
}
