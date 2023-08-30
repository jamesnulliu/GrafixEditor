#include "EntryPoint.h"

class GraphicsEditor : public Graphics::Application
{
    // Write somthing here...
};

Graphics::Application* Graphics::CreateApplication()
{
    return new GraphicsEditor();
}