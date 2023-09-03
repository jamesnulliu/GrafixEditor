#include "EntryPoint.h"

class DraftingBoard : public GE::Application
{
    // Write somthing here...
};

GE::Application* GE::CreateApplication()
{
    return new DraftingBoard();
}