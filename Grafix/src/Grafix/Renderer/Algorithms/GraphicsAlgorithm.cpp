#include "pch.h"
#include "GraphicsAlgorithm.h"

#include "LineAlgorithm.h"

namespace Grafix
{
	static uint32_t* s_PixelData = nullptr;
	static uint32_t s_CanvasWidth = 0, s_CanvasHeight = 0;

	static uint32_t s_LineWidth = 1;
	static LineStyleType s_LineStyle = LineStyleType::Solid;

	static std::vector<char> s_Pattern{  };
	static int s_Index = 0;

	void GraphicsAlgorithm::UpdatePixelData(uint32_t* pixelData, uint32_t width, uint32_t height)
	{
		s_PixelData = pixelData;
		s_CanvasWidth = width;
		s_CanvasHeight = height;
	}

	void GraphicsAlgorithm::SetLineProperties(float lineWidth, LineStyleType lineStyle)
	{
		s_LineWidth = (uint32_t)(lineWidth + 0.5);
		s_LineStyle = lineStyle;

		s_Index = 0;

		if (s_LineStyle == LineStyleType::Dashed)
		{
			s_Pattern.clear();
			for (int i = 0; i <= 3 * s_LineWidth; i++)
				s_Pattern.push_back('0');
			for (int i = 0; i <= 4 * s_LineWidth * 3; i++)
				s_Pattern.push_back('1');
		}

		if (s_LineStyle == LineStyleType::Dotted)
		{
			s_Pattern.clear();
			for (int i = 0; i <= 3 * s_LineWidth; i++)
				s_Pattern.push_back('0');
			for (int i = 0; i <= s_LineWidth / 2; i++)
				s_Pattern.push_back('1');
			for (int i = 0; i <= 3 * s_LineWidth; i++)
				s_Pattern.push_back('0');
			for (int i = 0; i <= 6 * s_LineWidth; i++)
				s_Pattern.push_back('1');
		}
	}

	void GraphicsAlgorithm::DrawSquare(const glm::vec2& center, float length, const glm::vec3& color)
	{
		for (int i = center.y - length / 2.0f; i < center.y + length / 2.0f; i++)
			for (int j = center.x - length / 2.0f; j < center.x + length / 2.0f; j++)
				SetSinglePixel(j, i, RGBToUint32(color));
	}

	uint32_t GraphicsAlgorithm::GetWidth()
	{
		return s_CanvasWidth;
	}

	uint32_t GraphicsAlgorithm::GetHeight()
	{
		return s_CanvasHeight;
	}

	void GraphicsAlgorithm::SetPixel(int x, int y, uint32_t colorValue, bool moveForward)
	{
		int halfWidth = (s_LineWidth - 1) >> 1;

		int xMin = x - halfWidth, xMax = xMin + s_LineWidth;
		int yMin = y - halfWidth, yMax = yMin + s_LineWidth;

		if (s_LineStyle == LineStyleType::Solid
			|| s_Pattern[s_Index % s_Pattern.size()] == '1')
		{
			for (int j = yMin; j < yMax; ++j)
				for (int i = xMin; i < xMax; ++i)
					SetSinglePixel(i, j, colorValue);
		}
		if (s_LineStyle != LineStyleType::Solid && moveForward)
			s_Index++;
	}


	void GraphicsAlgorithm::SetSinglePixel(int x, int y, uint32_t colorValue)
	{
		if (x >= 0 && x < s_CanvasWidth && y >= 0 && y < s_CanvasHeight)
			s_PixelData[x + y * s_CanvasWidth] = colorValue;
	}

	uint32_t GraphicsAlgorithm::GetPixelValue(int x, int y)
	{
		return s_PixelData[x + y * s_CanvasWidth];
	}
}