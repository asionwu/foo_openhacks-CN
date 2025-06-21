#pragma once
#include "ui_dialog_base.h"

class UIPrefAdvancedDialog : public UIPrefDialogBase<UIPrefAdvancedDialog, IDD_PREF_ADVANCED>
{
public:
    UIPrefAdvancedDialog(preferences_page_callback::ptr cb) : UIPrefDialogBase(cb)
    {
    }

    BEGIN_MSG_MAP(UIPrefAdvancedDialog)
        CHAIN_MSG_MAP(UIPrefDialogBase)
        MSG_WM_COMMAND(OnCommand)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

  protected:
    void OnInitDialog() override;
    void OnApply() override;

  private:
    void OnCommand(UINT code, int id, CWindow ctrl);

    void LoadUIState();
    void SaveUIState();

    void ApplySettings();

private:
    CComboBox mComboMenuBar;
    CComboBox mComboStatusBar;
};
