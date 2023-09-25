#include "Grafix.h"
#include "Grafix/Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Grafix
{
    class GrafixEditor : public Application
    {
    public:
        GrafixEditor(const AppSpecification& appSpec) :
            Application(appSpec)
        {
            PushLayer(new EditorLayer());
        }

        virtual ~GrafixEditor() = default;
    };

    Application* CreateApplication()
    {
        AppSpecification appSpec;
        appSpec.Name = "Grafix Editor";
        appSpec.Width = 1440;
        appSpec.Height = 810;

        return new GrafixEditor(appSpec);
    }
}