#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Grafix
{
    struct Component
    {
        Component() = default;
        Component(const Component&) = default;
        virtual ~Component() = default;
    };

    struct TransformComponent final : public Component
    {
        glm::vec3 Translation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale{ 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation) : Translation(translation) {}

        glm::mat4 GetTransform() const
        {
            // TODO: Implement translate, rotate and scale ourselves.
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), Translation);

            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), Scale);

            return translationMatrix * rotationMatrix * scaleMatrix;
        }
    };

    struct TagComponent final : public Component
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag) : Tag(tag) {}
    };

    struct UUIDComponent final : public Component
    {
        // Not the correct one
        ////UUID uuid;

        UUIDComponent() = default;
        UUIDComponent(const UUIDComponent&) = default;
        ////UUIDComponent(const UUID& uuid) : uuid(uuid) {}
    };

    // -----------------------------------------------------------------
    // ---------------------- Renderer Components ----------------------
    // -----------------------------------------------------------------

    enum class LineStyle
    {
        Solid, Dashed
    };

    struct LineRendererComponent final : public Component
    {
        glm::vec3 P0{ 200.0f, 300.0f, 0.0f };
        glm::vec3 P1{ 300.0f, 300.0f, 0.0f };

        glm::vec4 Color{ 0.8f, 0.8f, 0.8f, 1.0f };

        float Width = 1.0f;
        LineStyle Style = LineStyle::Solid;

        float DashLength = 1.0f;

        LineRendererComponent() = default;
        LineRendererComponent(const LineRendererComponent&) = default;
    };

    struct CircleRendererComponent final : public Component
    {
        glm::vec3 Center{ 250.0f, 300.0f, 0.0f };
        float Radius = 30.0f;

        glm::vec4 Color{ 0.8f, 0.8f, 0.8f, 1.0f };

        CircleRendererComponent() = default;
        CircleRendererComponent(const CircleRendererComponent&) = default;
    };

    struct ArcRendererComponent final : public Component
    {
        glm::vec3 Center{ 300.0f, 300.0f, 0.0f };
        glm::vec3 BeginPoint{ 350.0f, 350.0f, 0.0f };
        glm::vec3 ThirdPoint{ 400.0f, 300.0f, 0.0f };
        bool Choice = false;
        glm::vec4 Color{ 0.8f, 0.8f, 0.8f, 1.0f };
        
        ArcRendererComponent() = default;
		ArcRendererComponent(const ArcRendererComponent&) = default;
    };

    // NOT DONE
    struct PolygonRendererComponent final : public Component
    {
        std::vector<glm::vec3> Vertices;
        bool Closed = false;

        PolygonRendererComponent() = default;
        PolygonRendererComponent(const PolygonRendererComponent&) = default;

        void AddVertex(const glm::vec3& vertex)
        {
            if (!Vertices.empty())
            {
                // The last vertex is the same as the new one.
                if (Vertices.back() == vertex)
                    return;

                // The first vertex is the same as the new one.
                if (Vertices.front() == vertex)
                {
                    Closed = true;
                    return;
                }
            }

            Vertices.push_back(vertex);
        }
    };
}
