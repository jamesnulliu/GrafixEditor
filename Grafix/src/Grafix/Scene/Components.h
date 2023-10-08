#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Grafix/Math/Transformation.h"

namespace Grafix
{
    struct TransformComponent final
    {
        glm::vec2 PivotPoint{ 0.0f, 0.0f };

        glm::vec2 Translation{ 0.0f, 0.0f };
        float Rotation = 0.0f;
        glm::vec2 Scale{ 1.0f, 1.0f };

        bool KeepRatio = true;

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;

        glm::mat3 GetTransformMatrix() const
        {
            glm::mat3 initialTranslationMatrix = Math::CalcTranslationMatrix(-PivotPoint);

            glm::mat3 translationMatrix = Math::CalcTranslationMatrix(Translation);
            glm::mat3 rotationMatrix = Math::CalcRotationMatrix(glm::radians(Rotation));
            glm::mat3 scaleMatrix = Math::CalcScalingMatrix(Scale);

            glm::mat3 transformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

            glm::mat3 finalTranslationMatrix = Math::CalcTranslationMatrix(PivotPoint);

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
        glm::vec2 P0{ 400.0f, 500.0f };
        glm::vec2 P1{ 500.0f, 500.0f };

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        float Width = 1.0f;
        LineStyle Style = LineStyle::Solid;

        float DashLength = 1.0f;

        LineComponent() = default;
        LineComponent(const LineComponent&) = default;
    };

    struct CircleComponent final
    {
        glm::vec2 Center{ 450.0f, 500.0f };
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
        glm::vec2 Center{ 450.0f, 500.0f };
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
        std::vector<glm::vec2> Vertices;
        bool IsClosed = false;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        PolygonComponent() = default;
        PolygonComponent(const PolygonComponent&) = default;
    };
}
