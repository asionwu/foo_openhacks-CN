#include "pch.h"
#include "win32_utils.h"
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

namespace Utility
{
bool IsCompositionEnabled()
{
    BOOL composition_enabled = FALSE;
    return SUCCEEDED(DwmIsCompositionEnabled(&composition_enabled)) && composition_enabled;
}

bool EnableWindowShadow(HWND wnd, bool enable)
{
    if (IsCompositionEnabled())
    {
        static const MARGINS shadow_state[2]{{0, 0, 0, 0}, {1, 1, 1, 1}};
        const bool result = SUCCEEDED(DwmExtendFrameIntoClientArea(wnd, &shadow_state[enable ? 1 : 0]));
        const DWORD policy = DWMNCRP_ENABLED;
        std::ignore = DwmSetWindowAttribute(wnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));
        return result;
    }

    return false;
}

} // namespace Utility
