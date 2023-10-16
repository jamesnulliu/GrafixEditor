#pragma once
#include "pch.h"
#include "PolygonAlgorithm.h"

namespace Grafix
{
	class SutherlandHodgman
	{
	public:
		static std::vector<glm::vec2> SutherHodgClip(const std::vector<glm::vec2>& vertices,const std::vector<glm::vec2>& clipArea);
	
		static std::vector<glm::vec2> Clip(std::vector<glm::vec2> vertices,const glm::vec2& point1,const glm::vec2& point2);
		static glm::vec2 Intersection(const glm::vec2& point1,const glm::vec2& point2,const glm::vec2& point3,const glm::vec2& point4);
	};
}