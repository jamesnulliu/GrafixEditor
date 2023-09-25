#pragma once

#include "Image.h"
#include "Grafix/Scene/Scene.h"
#include "EditorCamera.h"

#include <memory>

namespace Grafix
{
    class Renderer
    {
    public:
        void Render(Scene& scene, EditorCamera& camera);

        std::shared_ptr<Image> GetImage() const { return m_Image; }
        void OnResize(uint32_t newWidth, uint32_t newHeight);

        void DrawLine(const LineRendererComponent& line);
        void DrawCircle(const CircleRendererComponent& circle);
        void DrawArc(const ArcRendererComponent& arc);
    private:
        std::shared_ptr<Image> m_Image = nullptr;
        uint32_t* m_Pixels = nullptr;

        Scene* m_ActiveScene = nullptr;
        EditorCamera* m_ActiveCamera = nullptr;

        std::vector<uint32_t> m_HorizontalIters;
        std::vector<uint32_t> m_VerticalIters;
    };
}
