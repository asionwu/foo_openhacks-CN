#include "pch.h"
#include "ui_pref_main_window.h"
#include "preferences_page_impl.h"
#include "hacks_core.h"
#include "hacks_vars.h"
#include "hacks_guids.h"

DECLARE_PREFERENCES_PAGE("Main window", UIPrefMainWindowDialog, 50.0, OpenHacksGuids::kMainWindowPageGuid, preferences_page::guid_display);

void UIPrefMainWindowDialog::OnInitDialog()
{
    SetHeaderFont(IDC_PREF_HEADER1);

    mComboFrameStyle.Attach(GetDlgItem(IDC_FRAME_STYLE));
    mComboFrameStyle.AddString(TEXT("Default"));
    mComboFrameStyle.AddString(TEXT("No caption"));
    mComboFrameStyle.AddString(TEXT("No border"));

    LoadUIState();
}

void UIPrefMainWindowDialog::OnApply()
{
    SaveUIState();
    ApplySettings();
}

void UIPrefMainWindowDialog::OnCommand(UINT code, int id, CWindow ctrl)
{
    if (code == CBN_SELCHANGE)
    {
        NotifyStateChanges(true);
    }
}

void UIPrefMainWindowDialog::LoadUIState()
{
    mComboFrameStyle.SetCurSel(OpenHacksVars::MainWindowFrameStyle);
}

void UIPrefMainWindowDialog::SaveUIState()
{
    int curSel = mComboFrameStyle.GetCurSel();
    if (curSel == CB_ERR)
        curSel = 0;
    OpenHacksVars::MainWindowFrameStyle = curSel;
}

void UIPrefMainWindowDialog::ApplySettings()
{
    auto& api = OpenHacksCore::Get();
    api.ApplyMainWindowFrameStyle(static_cast<WindowFrameStyle>((int32_t)OpenHacksVars::MainWindowFrameStyle));
}
