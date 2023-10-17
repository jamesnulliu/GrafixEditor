#include "pch.h"
#include "LineAlgorithm.h"
#include "Grafix/Renderer/Renderer.h"
#include "Grafix/Math/Transformation.h"

namespace Grafix
{
	void LineAlgorithm::Draw(const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, LineStyle style, float dashLength, float width)
	{
		Midpoint(p0, p1, color, style, (uint32_t)dashLength, width);
		////Bresenham(p0, p1, color, style, (uint32_t)dashLength);
	}

	void LineAlgorithm::Midpoint(const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, LineStyle style, uint32_t dashLength, float width)
	{
		int x0 = (int)p0.x, y0 = (int)p0.y;
		int x1 = (int)p1.x, y1 = (int)p1.y;

		// If |k| > 1, swap x and y.
		bool XYSwapped = std::abs(y1 - y0) > std::abs(x1 - x0);
		if (XYSwapped)
		{
			XYSwapped = true;
			std::swap(x0, y0);
			std::swap(x1, y1);
		}

		// If p0 is to the right of p1, swap them.
		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		int sy = y0 < y1 ? 1 : -1;
		int a = (y0 - y1) * sy;
		int b = x1 - x0;

		int d = (a << 1) + b;
		int d1 = a << 1, d2 = (a + b) << 1;

		uint32_t gapLength = std::max(dashLength / 5, (uint32_t)1);
		std::vector<uint8_t> pattern(dashLength + gapLength, 0);
		for (uint32_t i = 0; i < dashLength; ++i)
			pattern[i] |= 1;

		if (style == LineStyle::Dotted)
		{
			const char* dot = "000011000011111111111";
			pattern.clear();
			pattern.resize(strlen(dot));
			for (uint32_t i = 0; i < pattern.size(); ++i)
				pattern[i] = dot[i] - '0';
		}

		int x = x0, y = y0;
		int count = 0;

		while (x <= x1)
		{
			// Set pixel
			if (style == LineStyle::Solid ||
				(style == LineStyle::Dashed && pattern[count % pattern.size()])
				|| (style == LineStyle::Dotted && pattern[count % pattern.size()]))
			{
				if (XYSwapped) {
					
					SetPixel(y, x, color,width);
				}

				else {
					
						SetPixel(x, y, color,width);
				}
			}

			if (d < 0)
				y += sy, d += d2;
			else
				d += d1;

			++x;
			++count;
		}

	}

	void LineAlgorithm::Bresenham(const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, LineStyle style, uint32_t dashLength)
	{
		int x0 = (int)p0.x, y0 = (int)p0.y;
		int x1 = (int)p1.x, y1 = (int)p1.y;

		// If |k| > 1, swap x and y.
		bool XYSwapped = std::abs(y1 - y0) > std::abs(x1 - x0);
		if (XYSwapped)
		{
			std::swap(x0, y0);
			std::swap(x1, y1);
		}

		// If p0 is to the right of p1, swap them.
		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		int dx = (int)(x1 - x0), dy = (int)(y1 - y0);
		int e = -dx;
		int sy = dy > 0 ? 1 : -1;
		dy = std::abs(dy);

		uint32_t gapLength = std::max(dashLength / 5, (uint32_t)1);
		std::vector<uint8_t> pattern(dashLength + gapLength, 0);
		for (uint32_t i = 0; i < dashLength; ++i)
			pattern[i] |= 1;

		int x = x0, y = y0;

		for (int i = 0; i <= dx; i++)
		{
			if (style == LineStyle::Solid ||
				(style == LineStyle::Dashed && pattern[i % pattern.size()]))
			{
				// Set pixel
				if (XYSwapped)
					SetPixel(y, x, color);
				else
					SetPixel(x, y, color);
			}

			++x;
			e += dy << 1;

			if (e >= 0)
			{
				y += sy;
				e -= dx << 1;
			}
		}
	}
}