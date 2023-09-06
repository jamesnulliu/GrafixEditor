#include <GraphicsEditor.h>
#include <GraphicsEditor/Core/EntryPoint.h>

class DraftingBoard : public GE::Application
{
public:
    DraftingBoard(const GE::AppSpecification& appSpec) :
        GE::Application(appSpec) {}
    virtual ~DraftingBoard() = default;

    // Write somthing here...
};

GE::Application* GE::CreateApplication()
{
    AppSpecification appSpec;
    appSpec.Name = "Drafting Board";
    appSpec.Width = 1280;
    appSpec.Height = 720;
    return new DraftingBoard(appSpec);
}