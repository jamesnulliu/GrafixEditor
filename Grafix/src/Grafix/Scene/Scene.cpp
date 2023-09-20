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
        line.GetStyle() = Grafix::LineStyle::Solid;

        m_Lines.push_back(line);
    }

    void Scene::OnUpdate()
    {
    }
}