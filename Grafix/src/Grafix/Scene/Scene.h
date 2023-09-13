#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace Grafix
{
    struct Rectangle
    {
        glm::vec2 Position = { 200.12f, 300.35f };
        float Width = 90.0f;
        float Height = 90.0f;
        glm::vec3 Color = { 0.271f, 0.427f, 0.461f };
    };

    class Scene
    {
    public:
        // TODO: Remove all of these
        Scene()
        {
            {
                Rectangle rect1{};
                rect1.Position = { 114.0f, 204.0f };
                rect1.Width = 422.0f;
                rect1.Height = 422.0f;
                rect1.Color = { 0.236f, 0.513f, 0.574f };
                m_Rectangles.push_back(std::move(rect1));
            }

            {
                Rectangle rect2{};
                rect2.Position = { 316.0f, 140.0f };
                rect2.Width = 264.0f;
                rect2.Height = 264.0f;
                rect2.Color = { 0.8f, 0.2f, 0.3f };
                m_Rectangles.push_back(std::move(rect2));
            }
        }

        ~Scene() = default;

        glm::vec3& GetBackgroundColor() { return m_BackgroundColor; }

        std::vector<Rectangle>& GetRectangles() { return m_Rectangles; }
    private:
        std::vector<Rectangle> m_Rectangles;
        glm::vec3 m_BackgroundColor{ 0.761f, 0.755f, 0.698f };
    };
}
