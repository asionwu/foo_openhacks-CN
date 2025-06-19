#include "pch.h"
#include "hacks_menu.h"

WNDPROC mainWindowOriginProc = nullptr;

namespace
{

static bool PopupMainMenu(HWND wnd)
{
    if (HMENU menu = OpenHacksMenu::Get().GenerateMenu())
    {
        POINT point = {};
        ClientToScreen(wnd, &point);
        const int32_t cmd = TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, point.x, point.y, 0, wnd, nullptr);
        OpenHacksMenu::Get().ExecuteMenuCommand(cmd);
        DestroyMenu(menu);
        return true;
    }

    return false;
}

static bool OnSysCommand(HWND wnd, WPARAM wp, LPARAM lp)
{
    UNREFERENCED_PARAMETER(lp);
    const auto cmd = static_cast<UINT>(wp & 0xFFF0);
    switch (cmd)
    {
    case SC_MOUSEMENU:
        return PopupMainMenu(wnd);

    default:
        break;
    }

    return false;
}

} // namespace

LRESULT CALLBACK HacksMainWindowProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_SYSCOMMAND:
        if (OnSysCommand(wnd, wp, lp))
            return 0;
        break;

    default:
        break;
    }

    return CallWindowProc(mainWindowOriginProc, wnd, msg, wp, lp);
}