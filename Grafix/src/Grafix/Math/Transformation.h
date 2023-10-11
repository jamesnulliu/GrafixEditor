#pragma once
#include "pch.h"

namespace Grafix
{
    /*
        NOTE: Matrices in glm library are column major, so the data should be stored as:
            [0][0] [1][0] [2][0] [3][0] ...
            [0][1] [1][1] [2][1] [3][1] ...
            [0][2] [1][2] [2][2] [3][2] ...
            [0][3] [1][3] [2][3] [3][3] ...
            ...    ...    ...    ...    ...
    */

    namespace Math
    {
        glm::mat3 CalcTranslationMatrix(const glm::vec2& translation);
        glm::mat3 CalcRotationMatrix(float rotation);
        glm::mat3 CalcScalingMatrix(const glm::vec2& scale);

        glm::vec2 Transform(const glm::mat3& matrix, const glm::vec2& point);
    }
}