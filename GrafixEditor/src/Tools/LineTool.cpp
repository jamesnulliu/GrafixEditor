#include "LineTool.h"

namespace Grafix
{
    void LineTool::OnUpdate()
    {
        if (!m_IsDrawing)
        {
            if (Input::IsMouseButtonPressed(MouseButton::Left))
            {
                m_IsDrawing = true;

                glm::vec3 mousePos{ Input::GetMousePosition(), 0.0f };
                m_LineSpec.P0 = mousePos;
            }
        } else
        {
            if (Input::IsMouseButtonPressed(MouseButton::Left))
            {
                glm::vec3 mousePos{ Input::GetMousePosition(), 0.0f };
                m_LineSpec.P1 = mousePos;
            } else
            {
                m_IsDrawing = false;
            }
        }
    }
}