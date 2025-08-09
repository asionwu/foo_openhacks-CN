#include "pch.h"
#include "win32_utils.h"
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

namespace Utility
{
int(WINAPI* pfnGetSystemMetricsForDpi)(int, UINT) = nullptr;
UINT(WINAPI* pfnGetDpiForSystem)() = nullptr;
UINT(WINAPI* pfnGetDpiForWindow)(HWND) = nullptr;
DPI_AWARENESS_CONTEXT(WINAPI* pfnGetThreadDpiAwarenessContext)() = nullptr;
DPI_AWARENESS(WINAPI* pfnGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value) = nullptr;

static std::once_flag staticLoadFlag;

static void LoadUtilityProc()
{
    // clang-format off
    std::call_once(staticLoadFlag, [&]()
    {
        if (HMODULE user32 = GetModuleHandleW(L"user32.dll"))
        {
            std::ignore = GetProcAddress(user32, "GetSystemMetricsForDpi", pfnGetSystemMetricsForDpi);
            std::ignore = GetProcAddress(user32, "GetDpiForSystem", pfnGetDpiForSystem);
            std::ignore = GetProcAddress(user32, "GetDpiForWindow", pfnGetDpiForWindow);
            std::ignore = GetProcAddress(user32, "GetThreadDpiAwarenessContext", pfnGetThreadDpiAwarenessContext);
            std::ignore = GetProcAddress(user32, "GetAwarenessFromDpiAwarenessContext", pfnGetAwarenessFromDpiAwarenessContext);
        }
    });
    // clang-format on
}

bool IsCompositionEnabled()
{
    BOOL composition_enabled = FALSE;
    return SUCCEEDED(DwmIsCompositionEnabled(&composition_enabled)) && composition_enabled;
}

bool EnableWindowShadow(HWND window, bool enable)
{
    if (IsCompositionEnabled())
    {
        static const MARGINS shadow_state[2]{{0, 0, 0, 0}, {1, 1, 1, 1}};
        const bool result = SUCCEEDED(DwmExtendFrameIntoClientArea(window, &shadow_state[enable ? 1 : 0]));
        const DWORD policy = DWMNCRP_ENABLED;
        std::ignore = DwmSetWindowAttribute(window, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));
        return result;
    }

    return false;
}

uint32_t GetDPI(HWND window)
{
    LoadUtilityProc();

    //if (window != nullptr && pfnGetDpiForWindow != nullptr)
    //    return pfnGetDpiForWindow(window);

    //if (pfnGetDpiForSystem != nullptr)
    //    return pfnGetDpiForSystem();

    if (HDC dc = GetDC(HWND_DESKTOP))
    {
        auto dpi = GetDeviceCaps(dc, LOGPIXELSX);
        ReleaseDC(window, dc);
        return static_cast<uint32_t>(dpi);
    }

    return USER_DEFAULT_SCREEN_DPI;
}

int32_t GetSystemMetricsForDpi(int32_t index, uint32_t dpi)
{
    LoadUtilityProc();

    if (pfnGetSystemMetricsForDpi != nullptr)
        return pfnGetSystemMetricsForDpi(index, dpi);

    const int32_t rc = GetSystemMetrics(index);
    if (dpi == USER_DEFAULT_SCREEN_DPI)
        return rc;

    if (pfnGetThreadDpiAwarenessContext != nullptr && pfnGetAwarenessFromDpiAwarenessContext != nullptr)
    {
        const auto awareness = pfnGetAwarenessFromDpiAwarenessContext(pfnGetThreadDpiAwarenessContext());
        if (awareness == DPI_AWARENESS_UNAWARE || awareness == DPI_AWARENESS_INVALID)
        {
            const double scaleFactor = static_cast<double>(dpi) / 96.0;
            return static_cast<int>(std::round(rc * scaleFactor));
        }
    }

    return rc;
}

} // namespace Utility
