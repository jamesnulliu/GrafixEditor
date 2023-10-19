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

    enum class LineStyleType : uint8_t
    {
        Solid, Dashed, Dotted
    };

    enum class LineAlgorithmType : uint8_t
    {
        Midpoint, Bresenham
    };

    struct LineComponent final
    {
        LineAlgorithmType Algorithm = LineAlgorithmType::Midpoint;

        glm::vec2 P0{ 400.0f, 500.0f };
        glm::vec2 P1{ 500.0f, 500.0f };

        float LineWidth = 5.0f;
        LineStyleType LineStyle = LineStyleType::Solid;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        LineComponent() = default;
        LineComponent(const LineComponent&) = default;

        glm::vec2 GetCenterOfGravity() const { return (P0 + P1) / 2.0f; }
    };

    struct CircleComponent final
    {
        glm::vec2 Center{ 450.0f, 500.0f };
        float Radius = 0.0f;

        float LineWidth = 5.0f;
        LineStyleType LineStyle = LineStyleType::Solid;

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

        float LineWidth = 5.0f;
        LineStyleType LineStyle = LineStyleType::Solid;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        ArcComponent() = default;
        ArcComponent(const ArcComponent&) = default;

        glm::vec2 GetCenterOfGravity() const
        {
            glm::vec2 Gpoint{ Center };
            float a1 = glm::mod(Angle1, 360.0f);
            float a2 = glm::mod(Angle2, 360.0f);

            float beginAngle = glm::min(a1, a2), endAngle = glm::max(a1, a2);

            if ((endAngle - beginAngle) > 180.0f)
            {
                std::swap(beginAngle, endAngle);
                endAngle += 360.0f;
            }

            float midAngle = (endAngle - beginAngle) / 2.0f;
            float midAngleRadian = glm::radians(midAngle);
            float distance = Radius * (glm::sin(midAngleRadian)) / midAngleRadian;

            if (Major)
                distance = distance - Radius;

            Gpoint.x = Center.x + distance * glm::cos(glm::radians(beginAngle + midAngle));
            Gpoint.y = Center.y + distance * glm::sin(glm::radians(beginAngle + midAngle));

            return Gpoint;
        }
    };

    struct PolygonComponent final
    {
        // The number of vertices must be at least 4
        std::vector<glm::vec2> Vertices;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        PolygonComponent() = default;
        PolygonComponent(const PolygonComponent&) = default;

        glm::vec2 GetCenterOfGravity() const
        {
            glm::vec2 centerOfGravity{ 0.0f, 0.0f };

            for (int i = 0; i < Vertices.size() - 1; ++i)
                centerOfGravity += Vertices[i];

            centerOfGravity /= ((float)Vertices.size() - 1);
            return centerOfGravity;
        }
    };

    struct FillComponent final
    {
        glm::vec2 FillPoint{ 450.0f, 500.0f };  // 种子点坐标
        glm::vec3 FillColor{ 0.2f, 0.2f, 0.2f };  // 填充颜色

        FillComponent() = default;
        FillComponent(const FillComponent&) = default;
    };

    enum class CurveAlgorithmType : uint8_t
    {
        Bezier, NURBS
    };

    struct CurveComponent final
    {
        std::vector<glm::vec2> ControlPoints{};
        int Order = 3;
        float Step = 0.001f;

        CurveAlgorithmType Algorithm = CurveAlgorithmType::Bezier;

        float LineWidth = 5.0f;
        LineStyleType LineStyle = LineStyleType::Solid;

        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };

        std::vector<float> Weights;  // weights.size() = controlPoints.size()
        std::vector<float> Knots;  // knots.size() = controlPoints.size() + Order

        CurveComponent() = default;
        CurveComponent(const CurveComponent&) = default;

        glm::vec2 GetCenterOfGravity() const
        {
            glm::vec2 centerOfGravity{ 0.0f, 0.0f };

            for (const auto& point : ControlPoints)
                centerOfGravity += point;

            centerOfGravity /= (float)ControlPoints.size();
            return centerOfGravity;
        }
    };

    struct ClipComponent final
    {
        glm::vec2 P0{ 400.0f, 500.0f };
        glm::vec2 P1{ 500.0f, 500.0f };

        ClipComponent() = default;
        ClipComponent(const ClipComponent&) = default;

    };
}
