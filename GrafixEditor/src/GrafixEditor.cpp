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
        appSpec.Width = 1700;
        appSpec.Height = 770;

        return new GrafixEditor(appSpec);
    }
}