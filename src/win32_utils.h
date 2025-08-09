#pragma once
#include <type_traits>

namespace Utility
{
template <typename F, typename = std::enable_if_t<std::is_function<typename std::remove_pointer<F>::type>::value>>
bool GetProcAddress(HMODULE h, const char* funcName, F& f)
{
    if (auto ptr = ::GetProcAddress(h, funcName))
    {
        f = reinterpret_cast<F>(ptr);
        return true;
    }

    f = nullptr;
    return false;
}

bool IsCompositionEnabled();
bool EnableWindowShadow(HWND window, bool enable);
uint32_t GetDPI(HWND window);
int32_t GetSystemMetricsForDpi(int32_t index, uint32_t dpi);
} // namespace Utility