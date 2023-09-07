#include "GraphicsEditor.h"
#include "GraphicsEditor/Core/EntryPoint.h"

#include "Panels/EditorLayer.h"

class DraftingBoard : public GE::Application
{
public:
    DraftingBoard(const GE::AppSpecification& appSpec) :
        GE::Application(appSpec)
    {
        PushLayer(new EditorLayer());
    }

    virtual ~DraftingBoard() = default;
};

GE::Application* GE::CreateApplication()
{
    AppSpecification appSpec;
    appSpec.Name = "Drafting Board";
    appSpec.Width = 1440;
    appSpec.Height = 810;

    return new DraftingBoard(appSpec);
}