#include "pch.h"
#include "ArcAlgorithm.h"

namespace Grafix {
	void ArcAlgorithm::Draw(const glm::vec2& center, const glm::vec2& beginPoint, 
		const glm::vec2& thirdPoint, const glm::vec4& color, const bool choice)
	{
		SetPixel((int)center.x, (int)center.y, color);
		SetPixel((int)beginPoint.x, (int)beginPoint.y, color);	
		SetPixel((int)thirdPoint.x, (int)thirdPoint.y, color);
		MidPointArc(center, beginPoint, thirdPoint , color, choice);
	}

	// BUG: When three points are on the same line, the arc is not drawn
	void ArcAlgorithm::MidPointArc(const glm::vec2& center, const glm::vec2& beginPoint, 
		const glm::vec2 thirdPoint, const glm::vec4& color , const bool choice) 
	{
		int radius = (int)glm::distance(center, beginPoint);
		glm::vec2 Midpoint;
		Midpoint.x = (int)(beginPoint.x + thirdPoint.x) / 2;
		Midpoint.y = (int)(beginPoint.y + thirdPoint.y) / 2;
		glm::vec3 f1 = GetLineArguments(center, beginPoint);
		glm::vec3 f2 = GetLineArguments(center, thirdPoint);
		int centerX = center.x, centerY = center.y;
		int a = 0, b = radius, e = 1 - radius;
		if (choice)
			SetMajorArcPixels(centerX, centerY, a, b, color, f1 ,f2, Midpoint);
		else
			SetMinorArcPixels(centerX, centerY, a, b, color, f1, f2, Midpoint);

		while (a <= b) 
		{
			if (e < 0)
				e += 2 * a + 3;
			else 
			{
				e += 2 * (a - b) + 5;
				b--;
			}
			a++;
			if (choice) 
				SetMajorArcPixels(centerX, centerY, a, b, color, f1, f2, Midpoint);
			else
				SetMinorArcPixels(centerX, centerY, a, b, color, f1, f2, Midpoint);
		}
	}

	void ArcAlgorithm::SetMajorArcPixels(int centerX, int centerY, int x, int y,
		const glm::vec4& color, const glm::vec3 f1, const glm::vec3 f2, const glm::vec2& midpoint)
	{
		bool position1 = IsPointAboveLine((int)midpoint.x, (int)midpoint.y, f1);
		bool position2 = IsPointAboveLine((int)midpoint.x, (int)midpoint.y, f2);
		if (!(IsPointAboveLine(centerX + x, centerY + y, f1) == position1 && IsPointAboveLine(centerX + x, centerY + y, f2) == position2))
			SetPixel(centerX + x, centerY + y, color);
		if (!(IsPointAboveLine(centerX + x, centerY - y, f1) == position1 && IsPointAboveLine(centerX + x, centerY - y, f2) == position2))
			SetPixel(centerX + x, centerY - y, color);
		if (!(IsPointAboveLine(centerX - x, centerY + y, f1) == position1 && IsPointAboveLine(centerX - x, centerY + y, f2) == position2))
			SetPixel(centerX - x, centerY + y, color);
		if (!(IsPointAboveLine(centerX - x, centerY - y, f1) == position1 && IsPointAboveLine(centerX - x, centerY - y, f2) == position2))
			SetPixel(centerX - x, centerY - y, color);
		if (!(IsPointAboveLine(centerX + y, centerY + x, f1) == position1 && IsPointAboveLine(centerX + y, centerY + x, f2) == position2))
			SetPixel(centerX + y, centerY + x, color);
		if (!(IsPointAboveLine(centerX + y, centerY - x, f1) == position1 && IsPointAboveLine(centerX + y, centerY - x, f2) == position2))
			SetPixel(centerX + y, centerY - x, color);
		if (!(IsPointAboveLine(centerX - y, centerY + x, f1) == position1 && IsPointAboveLine(centerX - y, centerY + x, f2) == position2))
			SetPixel(centerX - y, centerY + x, color);
		if (!(IsPointAboveLine(centerX - y, centerY - x, f1) == position1 && IsPointAboveLine(centerX - y, centerY - x, f2) == position2))
			SetPixel(centerX - y, centerY - x, color);			

	}

	void ArcAlgorithm::SetMinorArcPixels(int centerX, int centerY, int x, int y,
		const glm::vec4& color, const glm::vec3 f1,const glm::vec3 f2, const glm::vec2 &midpoint) 
	{
		bool position1 = IsPointAboveLine((int)midpoint.x, (int)midpoint.y ,f1);
		bool position2 = IsPointAboveLine((int)midpoint.x, (int)midpoint.y ,f2);
		if (IsPointAboveLine(centerX + x, centerY + y, f1) == position1 && IsPointAboveLine(centerX + x, centerY + y, f2) == position2)
			SetPixel(centerX + x, centerY + y, color);
		if (IsPointAboveLine(centerX + x, centerY - y, f1) == position1 && IsPointAboveLine(centerX + x, centerY - y, f2) == position2)
			SetPixel(centerX + x, centerY - y, color);
		if (IsPointAboveLine(centerX - x, centerY + y, f1) == position1 && IsPointAboveLine(centerX - x, centerY + y, f2) == position2)
			SetPixel(centerX - x, centerY + y, color);
		if (IsPointAboveLine(centerX - x, centerY - y, f1) == position1 && IsPointAboveLine(centerX - x, centerY - y, f2) == position2)
			SetPixel(centerX - x, centerY - y, color);
		if(IsPointAboveLine(centerX + y, centerY + x, f1) == position1 && IsPointAboveLine(centerX + y, centerY + x, f2) == position2)
			SetPixel(centerX + y, centerY + x, color);
        if (IsPointAboveLine(centerX + y, centerY - x, f1) == position1 && IsPointAboveLine(centerX + y, centerY - x, f2) == position2)
			SetPixel(centerX + y, centerY - x, color);
		if (IsPointAboveLine(centerX - y, centerY + x, f1) == position1 && IsPointAboveLine(centerX - y, centerY + x, f2) == position2)
			SetPixel(centerX - y, centerY + x, color);
		if (IsPointAboveLine(centerX - y, centerY - x, f1) == position1 && IsPointAboveLine(centerX - y, centerY - x, f2) == position2)
			SetPixel(centerX - y, centerY - x, color);
	}

	glm::vec3 ArcAlgorithm::GetLineArguments(const glm::vec2& beginPoint, const glm::vec2& endPoint)
	{
		glm::vec3 arguments;
		arguments.x = endPoint.y - beginPoint.y;
		arguments.y = beginPoint.x - endPoint.x;
		arguments.z = endPoint.x * beginPoint.y - beginPoint.x * endPoint.y;
		return arguments;
	}

	bool ArcAlgorithm::IsPointAboveLine(int x, int y, const glm::vec3& lineArguments) 
	{
		return lineArguments.x * x + lineArguments.y * y + lineArguments.z < 0;
	}
}