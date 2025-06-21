// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "hacks_version.h"
#include <libPPUI/Controls.h>

BOOL APIENTRY DllMain(HMODULE mod, DWORD reason, LPVOID /*reserved*/)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(mod);
        CSeparator::Register();
        break;
    case DLL_PROCESS_DETACH:
        break;
    default:
        break;
    }
    return TRUE;
}

namespace
{
DECLARE_COMPONENT_VERSION_COPY("OpenHacks", HACKS_VERSION, GetOpenHacksAbout())
//VALIDATE_COMPONENT_FILENAME("foo_openhacks.dll")
} // namespace