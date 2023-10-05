#pragma once

#include "Image.h"

#include "Grafix/Scene/Scene.h"
#include "Grafix/Scene/Entity.h"

#include "EditorCamera.h"
#include "Algorithms/LineAlgorithm.h"

#include <memory>

namespace Grafix
{
    class Renderer
    {
    public:
        std::shared_ptr<Image> GetImage() const { return m_Image; }
        void OnResize(uint32_t newWidth, uint32_t newHeight);

        void Render(Scene& scene, EditorCamera& camera);
    private:
        void DrawLine(Entity entity);
        void DrawCircle(Entity entity);
        void DrawArc(Entity entity);
        void DrawPolygon(Entity entity);

        std::shared_ptr<Image> m_Image = nullptr;
        uint32_t* m_Pixels = nullptr;
    };
}
