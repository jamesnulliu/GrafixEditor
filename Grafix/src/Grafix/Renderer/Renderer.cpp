#include "pch.h"
#include "Renderer.h"

#include "Grafix/Utils/ColorConvert.hpp"
#include "Grafix/Math/Transformation.h"

#include "Algorithms/LineAlgorithm.h"
#include "Algorithms/CircleAlgorithm.h"
#include "Algorithms/ArcAlgorithm.h"
#include "Algorithms/PolygonAlgorithm.h"
#include "Algorithms/SeedFillAlgorithm.h"
#include "Algorithms/CurveAlgorithm.h"

namespace Grafix
{
	static glm::mat3 s_ViewMatrix = glm::mat3(1.0f);

	void Renderer::OnResize(uint32_t newWidth, uint32_t newHeight)
	{
		if (m_Image)
		{
			if (m_Image->GetWidth() == newWidth && m_Image->GetHeight() == newHeight)
				return;

			m_Image->Resize(newWidth, newHeight);
			GF_INFO("Image resized: {0}x{1}", m_Image->GetWidth(), m_Image->GetHeight());
		}
		else
		{
			m_Image = std::make_shared<Image>(newWidth, newHeight, ImageFormat::RGBA);
			GF_INFO("Image created: {0}x{1}", m_Image->GetWidth(), m_Image->GetHeight());
		}

		delete[] m_Pixels;
		m_Pixels = new uint32_t[newWidth * newHeight];
	}

	void Renderer::BeginScene(const Camera& camera)
	{
		s_ViewMatrix = camera.GetViewMatrix();
		GraphicsAlgorithm::UpdatePixelData(m_Pixels, m_Image->GetWidth(), m_Image->GetHeight());

		// Clear color
		std::fill(m_Pixels, m_Pixels + m_Image->GetWidth() * m_Image->GetHeight(), RGBToUint32(m_ClearColor));
	}

	void Renderer::EndScene()
	{
		m_Image->SetPiexels(m_Pixels);
	}

	void Renderer::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, LineStyle style, float dashLength)
	{
		DrawLine(TransformComponent(), p0, p1, color, style, dashLength);
	}

	void Renderer::DrawLine(const TransformComponent& transform, const glm::vec2& p0, const glm::vec2& p1, const glm::vec3& color, LineStyle style, float dashLength, float width)
	{
		LineAlgorithm::Draw(
			Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), p0)),
			Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), p1)),
			color, style, dashLength, width
		);
	}

	void Renderer::DrawCircle(const glm::vec2& center, float radius, const glm::vec3& color)
	{
		DrawCircle(TransformComponent(), center, radius, color);
	}

	void Renderer::DrawCircle(const TransformComponent& transform, const glm::vec2& center, float radius, const glm::vec3& color)
	{
		CircleAlgorithm::Draw(
			Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), center)),
			radius, color
		);
	}

	void Renderer::DrawArc(const glm::vec2& center, float radius, float angle1, float angle2, bool major, const glm::vec3& color)
	{
		DrawArc(TransformComponent(), center, radius, angle1, angle2, major, color);
	}

	void Renderer::DrawArc(const TransformComponent& transform, const glm::vec2& center, float radius, float angle1, float angle2, bool major, const glm::vec3& color)
	{
		glm::vec2 angle{ 0,0 };
		auto newCenter = Math::Transform(transform.GetTransformMatrix(), center);

		glm::vec2 delta1 = radius *
			glm::vec2{ glm::cos(glm::radians(angle1)), glm::sin(glm::radians(angle1)) } ;

		glm::vec2 delta2 = radius *
			glm::vec2{ glm::cos(glm::radians(angle2)), glm::sin(glm::radians(angle2)) } ;

		delta1 = Math::Transform(transform.GetTransformMatrix(), delta1 + center) - newCenter;
		delta2 = Math::Transform(transform.GetTransformMatrix(), delta2 + center) - newCenter;

		angle[0] = glm::atan(delta1.y , delta1.x);
		angle[1] = glm::atan(delta2.y, delta2.x);
		angle = glm::degrees(angle);

		ArcAlgorithm::Draw(newCenter, radius, angle[0], angle[1], major, color);
	}

	void Renderer::DrawPolygon(const std::vector<glm::vec2>& vertices, const glm::vec3& color)
	{
		DrawPolygon(TransformComponent(), vertices, color);
	}

	void Renderer::DrawPolygon(const TransformComponent& transform, const std::vector<glm::vec2>& vertices, const glm::vec3& color)
	{
		if (vertices.size() == 0 || vertices[0] != vertices[vertices.size() - 1])
		{

		}
		else
		{
			std::vector<glm::vec2> transformedVertices(vertices);

			for (auto& vertex : transformedVertices)
				vertex = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), vertex));

			PolygonAlgorithm::Draw(transformedVertices, color);
		}
	}

	void Renderer::DrawCurve(const std::vector<glm::vec2>& controlPoints,
		const glm::vec3& color, int order, float step,
		const std::vector<float>& knots, const std::vector<float>& weights)
	{
		if (controlPoints.size() >= order)
			CurveAlgorithm::NURBS(controlPoints, order, step, color, knots, weights);
		////CurveAlgorithm::Bezier(controlPoints, step, color);
	}

	void Renderer::DrawCross(const glm::vec2& center, float radius, const glm::vec3& color, LineStyle lineStyle, float dashLength)
	{
		DrawCross(TransformComponent(), center, radius, color, lineStyle, dashLength);
	}

	void Renderer::DrawCross(const TransformComponent& transform, const glm::vec2& center, float radius, const glm::vec3& color, LineStyle lineStyle, float dashLength)
	{
		glm::vec2 transformedCenter = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), center));

		LineAlgorithm::Draw(
			{ transformedCenter.x - radius, transformedCenter.y },
			{ transformedCenter.x + radius, transformedCenter.y },
			color, lineStyle, dashLength
		);

		LineAlgorithm::Draw(
			{ transformedCenter.x, transformedCenter.y - radius },
			{ transformedCenter.x, transformedCenter.y + radius },
			color, lineStyle, dashLength
		);
	}

	void Renderer::DrawSquare(const glm::vec2& center, float length, const glm::vec3& color)
	{
		for (int i = center.y - length / 2.0f; i < center.y + length / 2.0f; i++)
		{
			for (int j = center.x - length / 2.0f; j < center.x + length / 2.0f; j++)
			{
				m_Pixels[(uint32_t)j + (uint32_t)i * m_Image->GetWidth()] = RGBToUint32(color);
			}
		}
	}

	void Renderer::Fill(const glm::vec2& seedPoint, const glm::vec3& fillColor)
	{
		uint32_t oldColor = m_Pixels[(uint32_t)seedPoint.x + (uint32_t)seedPoint.y * m_Image->GetWidth()];
		SeedFillAlgorithm::Fill(seedPoint, RGBToUint32(fillColor), oldColor);
	}
}