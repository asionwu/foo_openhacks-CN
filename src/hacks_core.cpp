#include "pch.h"
#include "hacks_core.h"
#include "hacks_vars.h"

// forward declares
extern WNDPROC mainWindowOriginProc;
LRESULT CALLBACK HacksMainWindowProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);

namespace
{
static HHOOK staticCallWndHook = nullptr;
static HHOOK staticGetMsgHook = nullptr;

static WNDPROC staticStatusBarOriginProc = nullptr;

FORCEINLINE void UninstallWindowHook(HHOOK& hook)
{
    if (hook != nullptr)
    {
        UnhookWindowsHookEx(hook);
        hook = nullptr;
    }
}

LRESULT CALLBACK OpenHacksStatusBarProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_NCDESTROY:
        SetWindowLongPtr(wnd, GWLP_WNDPROC, (LONG_PTR)staticStatusBarOriginProc);
        break;

    case WM_WINDOWPOSCHANGING:
        if (auto wpos = (PWINDOWPOS)lp)
        {
            if (OpenHacksVars::ShowStatusBar == false)
            {
                wpos->cy = 0;
                return 0;
            }
        }
        break;

    default:
        break;
    }

    return CallWindowProc(staticStatusBarOriginProc, wnd, msg, wp, lp);
}

LRESULT CALLBACK OpenHacksCallWndProc(int code, WPARAM wp, LPARAM lp)
{
    if (code >= HC_ACTION)
    {
        const auto pcwps = reinterpret_cast<PCWPSTRUCT>(lp);
        switch (pcwps->message)
        {
        case WM_CREATE:
        {
            wchar_t className[MAX_PATH] = {};
            GetClassNameW(pcwps->hwnd, className, ARRAYSIZE(className));
            if (className == kDUIMainWindowClassName)
            {
                mainWindowOriginProc = (WNDPROC)SetWindowLongPtr(pcwps->hwnd, GWLP_WNDPROC, (LONG_PTR)HacksMainWindowProc);
            }
            break;
        }

        default:
            break;
        }
    }

    return CallNextHookEx(staticCallWndHook, code, wp, lp);
}

LRESULT CALLBACK OpenHacksGetMessageProc(int code, WPARAM wp, LPARAM lp)
{
    if (code >= HC_ACTION && (UINT)wp == PM_REMOVE)
    {
        const auto msg = (LPMSG)(lp);
        // TODO
        UNREFERENCED_PARAMETER(msg);
    }

    return CallNextHookEx(staticGetMsgHook, code, wp, lp);
}

} // namespace

OpenHacksCore& OpenHacksCore::Get()
{
    static OpenHacksCore core;
    return core;
}

void OpenHacksCore::Initialize()
{
    // check init errors.
    if (mInitErrors != NoError)
    {
        pfc::string8_fast errorMessage;
        if (mInitErrors & IncompatibleComponentInstalled)
        {
            errorMessage << "\nOpenHacks is not compatible with UIHacks.";
        }

        if (mInitErrors & HooksInstallError)
        {
            errorMessage << "\nfailed to install windows hook: " << format_win32_error(mInstallHooksWin32Error) << "(0x"
                         << pfc::format_hex(mInstallHooksWin32Error, 8) << ")";
        }

        popup_message_v2::g_complain(core_api::get_main_window(), "OpenHacks init failed", errorMessage);
        return;
    }

    if (HWND window = core_api::get_main_window())
    {
        if (HWND rebarWindow = FindWindowExW(window, nullptr, kDUIRebarWindowClassName.data(), nullptr))
        {
            mRebarWindow = rebarWindow;
            mMainMenuWindow = FindWindowExW(mRebarWindow, nullptr, kDUIMainMenuBandClassName.data(), nullptr);

            if (OpenHacksVars::ShowMainMenu == false)
            {
                ShowOrHideMenuBar(false);
            }
        }

        if (HWND statusBar = FindWindowExW(window, nullptr, kDUIStatusBarClassName.data(), nullptr))
        {
            mStatusBar = statusBar;
            staticStatusBarOriginProc = (WNDPROC)SetWindowLongPtr(statusBar, GWLP_WNDPROC, (LONG_PTR)OpenHacksStatusBarProc);

            if (OpenHacksVars::ShowStatusBar == false)
            {
                SendMessage(window, WM_SIZE, 0, 0);
            }
        }
    }
}

void OpenHacksCore::Finalize()
{
    UninstallWindowHooks();
}

bool OpenHacksCore::InstallWindowHooks()
{
    if (InstallWindowHooksInternal())
        return true;

    mInstallHooksWin32Error = GetLastError();
    mInitErrors |= HooksInstallError;
    UninstallWindowHooks();
    return false;
}

bool OpenHacksCore::InstallWindowHooksInternal()
{
    HINSTANCE hmod = core_api::get_my_instance();
    const DWORD threadId = GetCurrentThreadId();

    staticCallWndHook = SetWindowsHookExW(WH_CALLWNDPROC, OpenHacksCallWndProc, hmod, threadId);
    if (staticCallWndHook == nullptr)
        return false;

    staticGetMsgHook = SetWindowsHookExW(WH_GETMESSAGE, OpenHacksGetMessageProc, hmod, threadId);
    if (staticGetMsgHook == nullptr)
        return false;

    return true;
}

void OpenHacksCore::UninstallWindowHooks()
{
    UninstallWindowHook(staticGetMsgHook);
    UninstallWindowHook(staticCallWndHook);
}

void OpenHacksCore::ToggleStatusBar()
{
    const bool value = !OpenHacksVars::ShowStatusBar;
    if (ShowOrHideStatusBar(value))
        OpenHacksVars::ShowStatusBar = value;
}

void OpenHacksCore::ToggleMenuBar()
{
    const bool value = !OpenHacksVars::ShowMainMenu;
    if (ShowOrHideMenuBar(value))
        OpenHacksVars::ShowMainMenu = value;
}

bool OpenHacksCore::ShowOrHideStatusBar(bool /*value*/)
{
    if (mStatusBar == nullptr)
        return false;

    SendMessage(core_api::get_main_window(), WM_SIZE, 0, 0);
    return true;
}

bool OpenHacksCore::ShowOrHideMenuBar(bool value)
{
    if (mRebarWindow == nullptr || mMainMenuWindow == nullptr)
        return false;

    if (IsMenuBarVisible() == value)
        return true;

    const UINT bandCount = (UINT)SendMessage(mRebarWindow, RB_GETBANDCOUNT, 0, 0);
    for (UINT i = 0; i < bandCount; ++i)
    {
        REBARBANDINFO rebarInfo = {};
        rebarInfo.cbSize = sizeof(rebarInfo);
        rebarInfo.fMask = RBBIM_CHILD;
        SendMessage(mRebarWindow, RB_GETBANDINFO, (WPARAM)i, (LPARAM)&rebarInfo);
        if (mMainMenuWindow == rebarInfo.hwndChild)
        {
            SendMessage(mRebarWindow, RB_SHOWBAND, (WPARAM)i, (LPARAM)value);
            return true;
        }
    }

    return false;
}

bool OpenHacksCore::CheckIncompatibleComponents()
{
    static wstring_view_t incompatibleComponents[] = {
        L"foo_ui_hacks.dll",
    };

    for (const auto& fileName : incompatibleComponents)
    {
        if (GetModuleHandleW(fileName.data()))
        {
            mInitErrors |= IncompatibleComponentInstalled;
            break;
        }
    }

    return mInitErrors == NoError;
}
