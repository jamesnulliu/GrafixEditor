#include "Grafix.h"
#include "Grafix/Core/EntryPoint.h"

#include "Panels/EditorLayer.h"

class GrafixEditor : public Grafix::Application
{
public:
    GrafixEditor(const Grafix::AppSpecification& appSpec) :
        Grafix::Application(appSpec)
    {
        PushLayer(new EditorLayer());
    }

    virtual ~GrafixEditor() = default;
};

Grafix::Application* Grafix::CreateApplication()
{
    AppSpecification appSpec;
    appSpec.Name = "Grafix Editor";
    appSpec.Width = 1440;
    appSpec.Height = 810;

    return new GrafixEditor(appSpec);
}