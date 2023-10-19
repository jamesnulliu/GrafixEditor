#include "pch.h"
#include "MidPointClip.h"

namespace Grafix
{
	std::vector<glm::vec2> MidPointClip::Clip(glm::vec2 beginPoint, glm::vec2 endPoint, const glm::vec2& leftBottom, const glm::vec2& rightTop)
	{
		std::vector<glm::vec2> result;
		int code1 = Encode(beginPoint, leftBottom, rightTop);
		int code2 = Encode(endPoint, leftBottom, rightTop);
		int code = 0, x = 0, y = 0;
		float epsilon = 0.1f;

		if (code1 == 0 && code2 == 0)
		{
			result.push_back(beginPoint);
			result.push_back(endPoint);
			return result;
		}

		if ((code1 & code2) != 0)
		{
			result.push_back(glm::vec2{ -1,-1 });
			result.push_back(glm::vec2{ -1,-1 });
			return result;
		}

		if (code1 != 0)
		{
			glm::vec2 begin = beginPoint;
			glm::vec2 end = endPoint;

			while (Distance(begin, end) >= epsilon)
			{
				glm::vec2 mid = glm::vec2{ (begin[0] + end[0]) / 2, (begin[1] + end[1]) / 2 };
				code = Encode(mid, leftBottom, rightTop);
				if ((code & code1) == 0)
				{
					end = mid;
				}
				else
				{
					begin = mid;
					code1 = code;
				}
			}
			beginPoint = begin;
		}

		if (code2 != 0)
		{
			glm::vec2 begin = beginPoint;
			glm::vec2 end = endPoint;

			while (Distance(begin, end) >= epsilon)
			{
				glm::vec2 mid = glm::vec2({ (begin[0] + end[0]) / 2, (begin[1] + end[1]) / 2 });
				code = Encode(mid, leftBottom, rightTop);
				if ((code & code2) == 0)
					begin = mid;
				else
				{
					end = mid;
					code2 = code;
				}
			}
			endPoint = end;
		}

		result.push_back(beginPoint);
		result.push_back(endPoint);
		return result;
	}

	int MidPointClip::Encode(glm::vec2 point, const glm::vec2& leftBottom, const glm::vec2& rightTop)
	{
		int code = 0;

		if (point.x < leftBottom.x)
			code |= LEFT;
		else if (point.x > rightTop.x)
			code |= RIGHT;
		if (point.y < leftBottom.y)
			code |= BOTTOM;
		else if (point.y > rightTop.y)
			code |= TOP;

		return code;
	}

	float MidPointClip::Distance(const glm::vec2& beginPoint, const glm::vec2& endPoint)
	{
		return sqrt(pow((endPoint.x - beginPoint.x), 2) + pow(endPoint.y - beginPoint.y, 2));
	}
}

