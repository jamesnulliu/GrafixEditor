#pragma once

#include "Image.h"

#include "Grafix/Scene/Scene.h"
#include "Grafix/Scene/Entity.h"

#include "EditorCamera.h"

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
        std::shared_ptr<Image> m_Image = nullptr;
        uint32_t* m_Pixels = nullptr;
    };
}
