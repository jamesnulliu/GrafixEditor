#pragma once

#include "Image.h"

#include <memory>

namespace Grafix
{
    class Renderer
    {
    public:
        Renderer() {}
    private:
        std::unique_ptr<Image> m_Image;
    };
}
