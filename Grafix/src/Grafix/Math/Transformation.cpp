#include "pch.h";
#include "Transformation.h"

namespace Grafix
{
    namespace Math
    {
        /*
            NOTE: Matrices in glm library are column major, so the data should be stored as:
                [0][0] [1][0] [2][0] [3][0] ...
                [0][1] [1][1] [2][1] [3][1] ...
                [0][2] [1][2] [2][2] [3][2] ...
                [0][3] [1][3] [2][3] [3][3] ...
                ...    ...    ...    ...    ...
        */

        glm::mat3 CalcTranslationMatrix(const glm::vec2& translation)
        {
            return glm::mat3{
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f ,0.0f,
                translation.x, translation.y, 1.0f
            };
        }

        glm::mat3 CalcRotationMatrix(float rotation)
        {
            float cosine = glm::cos(rotation);
            float sine = glm::sin(rotation);

            return glm::mat3{
                cosine, sine, 0.0f,
                -sine, cosine, 0.0f,
                0.0f, 0.0f, 1.0f
            };
        }

        glm::mat3 CalcScalingMatrix(const glm::vec2& scale)
        {
            return glm::mat3{
                scale.x, 0.0f, 0.0f,
                0.0f, scale.y, 0.0f,
                0.0f, 0.0f, 1.0f
            };
        }

        glm::vec2 Math::Transform(const glm::mat3& matrix, const glm::vec2& point)
        {
            return glm::vec2(matrix * glm::vec3(point, 1.0f));
        }
    }
}