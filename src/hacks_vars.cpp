#include "pch.h"
#include "hacks_vars.h"

namespace OpenHacksVars
{
// {807863EA-CD09-4AEC-A4E1-69A607DCCF15}
static const GUID cfg_guid_show_main_menu = {0x807863ea, 0xcd09, 0x4aec, {0xa4, 0xe1, 0x69, 0xa6, 0x7, 0xdc, 0xcf, 0x15}};
// {AED3366F-C49D-4A57-9B22-1033C0C46D94}
static const GUID cfg_guid_show_status_bar = {0xaed3366f, 0xc49d, 0x4a57, {0x9b, 0x22, 0x10, 0x33, 0xc0, 0xc4, 0x6d, 0x94}};

cfg_bool ShowMainMenu(cfg_guid_show_main_menu, true);
cfg_bool ShowStatusBar(cfg_guid_show_status_bar, true);
} // namespace OpenHacksVars