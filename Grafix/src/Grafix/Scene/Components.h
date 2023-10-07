#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Grafix/Math/TransformAlgorithm.h"

namespace Grafix
{
    ////struct SolidColorComponent final
    ////{
    ////    glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

    ////    SolidColorComponent() = default;
    ////    SolidColorComponent(const SolidColorComponent&) = default;
    ////    SolidColorComponent(const glm::vec4& color) : Color(color) {}
    ////};

    struct TransformComponent final
    {
        glm::vec3 PivotPoint{ 0.0f, 0.0f, 0.0f };

        glm::vec3 Translation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale{ 1.0f, 1.0f, 1.0f };

        bool KeepRatio = true;

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;

        // TODO: Implement translate, rotate and scale ourselves.
        glm::mat4 GetTransformMatrix() const
        {

            //glm::mat4 initialTranslationMatrix = glm::translate(glm::mat4(1.0f), -PivotPoint);
            glm::mat4 initialTranslationMatrix = Math::Translate(glm::mat4(1.0f), -PivotPoint);
            
            //glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), Translation);
            glm::mat4 translationMatrix = Math::Translate(glm::mat4(1.0f), Translation);

            glm::vec3 rotation = glm::radians(Rotation);

            /*glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));*/
            glm::mat4 rotationMatrix = Math::Rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
                * Math::Rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
                * Math::Rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            
            //glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), Scale);
            glm::mat4 scaleMatrix = Math::Scale(glm::mat4(1.0f), Scale);

            glm::mat4 transformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

            //glm::mat4 finalTranslationMatrix = glm::translate(glm::mat4(1.0f), PivotPoint);
            glm::mat4 finalTranslationMatrix = Math::Translate(glm::mat4(1.0f), PivotPoint);
            return finalTranslationMatrix * transformMatrix * initialTranslationMatrix;
        }
    };

    struct TagComponent final
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag) : Tag(tag) {}
    };

    // -----------------------------------------------------------------
    // ---------------------- Renderer Components ----------------------
    // -----------------------------------------------------------------

    enum class LineStyle
    {
        Solid, Dashed
    };

    struct LineComponent final
    {
        glm::vec3 P0{ 400.0f, 500.0f, 0.0f };
        glm::vec3 P1{ 500.0f, 500.0f, 0.0f };

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        float Width = 1.0f;
        LineStyle Style = LineStyle::Solid;

        float DashLength = 1.0f;

        LineComponent() = default;
        LineComponent(const LineComponent&) = default;
    };

    struct CircleComponent final
    {
        glm::vec3 Center{ 450.0f, 500.0f, 0.0f };
        float Radius = 0.0f;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        // Aux
        bool ShowCenter = false;
        bool ShowRadius = false;

        CircleComponent() = default;
        CircleComponent(const CircleComponent&) = default;
    };

    struct ArcComponent final
    {
        glm::vec3 Center{ 450.0f, 500.0f, 0.0f };
        float Radius = 50.0f;
        float Angle1 = -30.0f;
        float Angle2 = 60.0f;
        bool Major = false;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        // Aux
        bool ShowCenter = false;
        bool ShowRadius = false;

        ArcComponent() = default;
        ArcComponent(const ArcComponent&) = default;
    };

    struct PolygonComponent final
    {
        std::vector<glm::vec3> Vertices;
        bool IsClosed = false;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        PolygonComponent() = default;
        PolygonComponent(const PolygonComponent&) = default;
    };
}
