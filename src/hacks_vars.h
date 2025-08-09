#pragma once
#include <algorithm>

#pragma pack(push)
#pragma pack(1)
struct PseudoCaptionParam
{
    enum Margins
    {
        Left = 0,
        Top,
        Right,
        Bottom,
        Max,
    };

    int32_t left = 0;
    int32_t top = 0;
    int32_t right = 0;
    int32_t bottom = 0;
    int32_t width = 0;
    int32_t height = 0;
    union
    {
        struct
        {
            bool left;
            bool top;
            bool right;
            bool bottom;
        };
        bool states[Margins::Max];
    } marginStates = {true, true, true, false};

    static_assert(sizeof(marginStates) == Margins::Max * sizeof(bool));

    int32_t reserved[25] = {};

    inline bool UseWidth() const
    {
        return marginStates.left == false || marginStates.right == false;
    }

    inline bool UseHeight() const
    {
        return marginStates.top == false || marginStates.bottom == false;
    }

    RECT ToRect(HWND wnd) const
    {
        RECT rect = {};
        GetClientRect(wnd, &rect);
        ClientToScreen(wnd, (LPPOINT)&rect);
        ClientToScreen(wnd, (LPPOINT)&rect + 1);

        RECT anchor = rect;
        GetWindowRect(wnd, &rect);
        rect.left = anchor.left;
        rect.right = anchor.right;
        rect.bottom = anchor.bottom;

        const int32_t ww = std::clamp(width, 0, static_cast<int32_t>(rect.right - rect.left));
        const int32_t wh = std::clamp(height, 0, static_cast<int32_t>(rect.bottom - rect.top));

        RECT rc = rect;
        if (marginStates.left)
            rc.left += left;

        if (marginStates.right)
            rc.right -= right;

        if (UseWidth())
        {
            if (marginStates.right)
                rc.left = rc.right - ww;
            else
                rc.right = rc.left + ww;
        }

        if (UseHeight())
        {
            if (marginStates.bottom)
                rc.top = rc.bottom - wh;
            else
                rc.bottom = rc.top + wh;
        }

        rc.left = std::clamp(rc.left, rect.left, rect.right);
        rc.right = std::clamp(rc.right, rc.left, rect.right);
        rc.top = std::clamp(rc.top, rect.top, rect.bottom);
        rc.bottom = std::clamp(rc.bottom, rc.top, rect.bottom);

        return rc;
    }
};
#pragma pack(pop)

namespace OpenHacksVars
{
static const char* kOpenHacksHelpURL = "https://github.com/ttsping/foo_openhacks";

extern cfg_bool ShowMainMenu;
extern cfg_bool ShowStatusBar;
extern cfg_int MainWindowFrameStyle;
extern cfg_struct_t<PseudoCaptionParam> PseudoCaptionSettings;

// runtime vars
extern uint32_t DPI;

void InitialseOpenHacksVars();
} // namespace OpenHacksVars