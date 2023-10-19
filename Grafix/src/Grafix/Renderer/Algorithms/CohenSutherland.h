#pragma once
//Use Cohen-Sutherland's algorithm to clip straight line segments

#include "pch.h"


namespace Grafix
{
	class CohenSutherland
	{
	public:
		static std::vector<glm::vec2> Clip(glm::vec2 beginPoint, glm::vec2 endPoint, const glm::vec2& leftBottom, const glm::vec2& rightTop);
	private:
		static const int LEFT = 1;
		static const int RIGHT = 2;
		static const int BOTTOM = 4;
		static const int TOP = 8;

		static int Encode(glm::vec2 point, const glm::vec2& leftBottom, const glm::vec2& rightTop);
	};
}