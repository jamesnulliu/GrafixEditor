#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Grafix/Math/Transformation.h"

namespace Grafix
{
    struct TransformComponent final
    {
        glm::vec2 Pivot{ 0.0f, 0.0f };

        glm::vec2 Translation{ 0.0f, 0.0f };
        float Rotation = 0.0f;
        glm::vec2 Scale{ 1.0f, 1.0f };

        bool KeepRatio = true;

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;

        glm::mat3 GetTransformMatrix() const
        {
            glm::mat3 initialTranslationMatrix = Math::CalcTranslationMatrix(-Pivot);

            glm::mat3 translationMatrix = Math::CalcTranslationMatrix(Translation);
            glm::mat3 rotationMatrix = Math::CalcRotationMatrix(glm::radians(Rotation));
            glm::mat3 scaleMatrix = Math::CalcScalingMatrix(Scale);

            glm::mat3 transformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

            glm::mat3 finalTranslationMatrix = Math::CalcTranslationMatrix(Pivot);

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

        glm::vec2 GetCenterOfGravity() const { return (P0 + P1) / 2.0f; }
    };

    struct CircleComponent final
    {
        glm::vec2 Center{ 450.0f, 500.0f };
        float Radius = 0.0f;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        CircleComponent() = default;
        CircleComponent(const CircleComponent&) = default;

        glm::vec2 GetCenterOfGravity() const { return Center; }
    };

    struct ArcComponent final
    {
        glm::vec2 Center{ 450.0f, 500.0f };
        float Radius = 50.0f;
        float Angle1 = -30.0f;
        float Angle2 = 60.0f;
        bool Major = false;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        ArcComponent() = default;
        ArcComponent(const ArcComponent&) = default;

        glm::vec2 GetCenterOfGravity() const
        {
            float midAngle = (Angle1 + Angle2) / 2.0f;
            glm::vec2 radiusVector = glm::vec2(glm::cos(glm::radians(midAngle)), glm::sin(glm::radians(midAngle)));
        }
    };

    struct PolygonComponent final
    {
        // The number of vertices must be at least 4
        std::vector<glm::vec2> Vertices;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        PolygonComponent() = default;
        PolygonComponent(const PolygonComponent&) = default;
    };

    struct SeedFillComponent final
    {
        glm::vec2 SeedPoint{ 450.0f, 500.0f };   // 种子点坐标
        glm::vec3 FillColor{ 0.2f, 0.2f, 0.2f };         // 填充颜色

        SeedFillComponent() = default;
        SeedFillComponent(const SeedFillComponent&) = default;
    };

    struct CurveComponent final
    {
        std::vector<glm::vec2> ControlPoints{};

        int Order = 3;
        float Step = 0.001f;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        CurveComponent() = default;
        CurveComponent(const CurveComponent&) = default;
    };
}
