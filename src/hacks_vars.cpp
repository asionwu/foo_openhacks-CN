#include "pch.h"
#include "hacks_vars.h"
#include "win32_utils.h"

namespace OpenHacksVars
{
// {807863EA-CD09-4AEC-A4E1-69A607DCCF15}
static const GUID cfg_guid_show_main_menu = {0x807863ea, 0xcd09, 0x4aec, {0xa4, 0xe1, 0x69, 0xa6, 0x7, 0xdc, 0xcf, 0x15}};
// {AED3366F-C49D-4A57-9B22-1033C0C46D94}
static const GUID cfg_guid_show_status_bar = {0xaed3366f, 0xc49d, 0x4a57, {0x9b, 0x22, 0x10, 0x33, 0xc0, 0xc4, 0x6d, 0x94}};
// {47377272-B6A9-4181-BE9A-F2F7817712CF}
static const GUID cfg_guid_main_window_frame_style = {0x47377272, 0xb6a9, 0x4181, {0xbe, 0x9a, 0xf2, 0xf7, 0x81, 0x77, 0x12, 0xcf}};
// {F8A0EA9D-3ACC-410A-ABF7-814065A8849D}
static const GUID cfg_guid_pseudo_caption = {0xf8a0ea9d, 0x3acc, 0x410a, {0xab, 0xf7, 0x81, 0x40, 0x65, 0xa8, 0x84, 0x9d}};

cfg_bool ShowMainMenu(cfg_guid_show_main_menu, true);
cfg_bool ShowStatusBar(cfg_guid_show_status_bar, true);
cfg_int MainWindowFrameStyle(cfg_guid_main_window_frame_style, 0);
cfg_struct_t<PseudoCaptionParam> PseudoCaptionSettings(cfg_guid_pseudo_caption);

// runtime vars
uint32_t DPI = USER_DEFAULT_SCREEN_DPI;

void InitialseOpenHacksVars()
{
    auto& pseudoCaption = PseudoCaptionSettings.get_value();
    if (pseudoCaption.height == 0)
    {
        auto height = Utility::GetSystemMetricsForDpi(SM_CYCAPTION, Utility::GetDPI(HWND_DESKTOP));
        height += Utility::GetSystemMetricsForDpi(SM_CYFRAME, Utility::GetDPI(HWND_DESKTOP));
        height += Utility::GetSystemMetricsForDpi(SM_CXPADDEDBORDER, Utility::GetDPI(HWND_DESKTOP));
        pseudoCaption.height = height;
    }
}

} // namespace OpenHacksVars