#include "pch.h"
#include "Scene.h"

namespace Grafix
{
    Scene::Scene()
    {
    }

    void Scene::AddLine()
    {
        Grafix::Line line("Line");
        line.GetPoint0() = glm::vec3{ 420.0f, 300.0f, 0.0f };
        line.GetPoint1() = glm::vec3{ 616.0f, 401.0f, 0.0f };
        line.GetSpriteRenderer().Color = glm::vec4{ 0.677f, 0.773f, 0.789f, 1.0f };
        line.GetWidth() = 1.0f;
        line.GetStyle() = Grafix::LineStyle::Dashed;
        line.GetDashLength() = 1.0f;

        m_Lines.push_back(line);
    }

    void Scene::AddCircle()
    {
        Grafix::Circle circle("Circle");
        circle.GetCenter() = glm::vec3{ 420.0f, 300.0f, 0.0f };
        circle.GetRadius() = 100.0f;
        circle.GetSpriteRenderer().Color = glm::vec4{ 0.677f, 0.773f, 0.789f, 1.0f };

        m_Circles.push_back(circle);
    }

    void Scene::OnUpdate()
    {
    }
}