#include "Grafix.h"
#include "Grafix/Core/EntryPoint.h"

#include "Panels/EditorLayer.h"

class DraftingBoard : public Grafix::Application
{
public:
    DraftingBoard(const Grafix::AppSpecification& appSpec) :
        Grafix::Application(appSpec)
    {
        PushLayer(new EditorLayer());
    }

    virtual ~DraftingBoard() = default;
};

Grafix::Application* Grafix::CreateApplication()
{
    AppSpecification appSpec;
    appSpec.Name = "Drafting Board";
    appSpec.Width = 1440;
    appSpec.Height = 810;

    return new DraftingBoard(appSpec);
}