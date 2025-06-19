#pragma once

enum HacksInitErrors : uint32_t
{
    NoError = 0,
    HooksInstallError = 1 << 0,
    IncompatibleComponentInstalled = 1 << 1,
};

class OpenHacksCore
{
  public:
    static OpenHacksCore& Get();

    FORCEINLINE bool HasInitError() const
    {
        return mInitErrors != HacksInitErrors::NoError;
    }

    void Initialize();
    void Finalize();

    bool InstallWindowHooks();
    void UninstallWindowHooks();

    void ToggleStatusBar();
    void ToggleMainMenu();

    bool CheckIncompatibleComponents();

  private:
    bool ShowOrHideMainMenu(bool value);
    bool InstallWindowHooksInternal();

  private:
    HWND mRebarWindow = nullptr;
    HWND mMainMenuWindow = nullptr;
    HWND mStatusBar = nullptr;
    uint32_t mInitErrors = HacksInitErrors::NoError;
    DWORD mInstallHooksWin32Error = ERROR_SUCCESS;
};