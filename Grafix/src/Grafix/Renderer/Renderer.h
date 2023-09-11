#pragma once

#include "Image.h"
#include "Grafix/Scene/Scene.h"

#include <glm/glm.hpp>
#include <memory>

namespace Grafix
{
    class Renderer
    {
    public:
        Renderer() = default;

        void Render(Scene& scene);

        std::shared_ptr<Image> GetImage() const { return m_Image; }
        void OnResize(uint32_t newWidth, uint32_t newHeight);
    private:
        std::shared_ptr<Image> m_Image = nullptr;
        uint32_t* m_Pixels = nullptr;

        Scene* m_ActiveScene = nullptr;
    };
}
