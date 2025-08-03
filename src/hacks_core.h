#pragma once

enum HacksInitErrors : uint32_t
{
    NoError = 0,
    HooksInstallError = 1 << 0,
    IncompatibleComponentInstalled = 1 << 1,
};

enum class WindowFrameStyle
{
    Default = 0,
    NoCaption,
    NoBorder,
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

    void ToggleStatusBar();
    void ToggleMenuBar();

    void ShowOrHideStatusBar(bool value);
    bool ShowOrHideMenuBar(bool value);

    bool CheckIncompatibleComponents();

    void ApplyMainWindowFrameStyle(WindowFrameStyle newStyle);

  private:
    FORCEINLINE static LRESULT CALLBACK StaticOpenHacksMainWindowProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        return Get().OpenHacksMainWindowProc(wnd, msg, wp, lp);
    }

    FORCEINLINE static LRESULT CALLBACK StaticOpenHacksCallWndProc(int code, WPARAM wp, LPARAM lp)
    {
        return Get().OpenHacksCallWndProc(code, wp, lp);
    }

    FORCEINLINE static LRESULT CALLBACK StaticOpenHacksGetMessageProc(int code, WPARAM wp, LPARAM lp)
    {
        return Get().OpenHacksGetMessageProc(code, wp, lp);
    }

    FORCEINLINE static LRESULT CALLBACK StaticOpenHacksStatusBarProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        return Get().OpenHacksStatusBarProc(wnd, msg, wp, lp);
    }

    FORCEINLINE bool IsMenuBarVisible() const
    {
        return mMainMenuWindow != nullptr && IsWindowVisible(mMainMenuWindow);
    }

    bool IsMainWindowBorderless() const;

    LRESULT OpenHacksMainWindowProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);
    LRESULT OpenHacksCallWndProc(int code, WPARAM wp, LPARAM lp);
    LRESULT OpenHacksGetMessageProc(int code, WPARAM wp, LPARAM lp);
    LRESULT OpenHacksStatusBarProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);

    void UninstallWindowHooks();
    bool InstallWindowHooksInternal();

    bool IsMainOrChildWindow(HWND wnd);

  private:
    // main window message handlers
    bool OnSysCommand(HWND wnd, WPARAM wp, LPARAM lp);
    LRESULT OnNCHitTest(HWND wnd, WPARAM wp, LPARAM lp);
    bool OnSetCursor(HWND wnd, WPARAM wp, LPARAM lp);
    bool OnSize(HWND wnd, WPARAM wp, LPARAM lp);
    // windows hook handlers
    void OnHookMouseMove(LPMSG msg);
    void OnHookLButtonDown(LPMSG msg);

  private:
    HWND mMainWindow = nullptr;
    HWND mRebarWindow = nullptr;
    HWND mMainMenuWindow = nullptr;
    HWND mStatusBar = nullptr;
    HHOOK mCallWndHook = nullptr;
    HHOOK mGetMsgHook = nullptr;
    WNDPROC mMainWindowOriginProc = nullptr;
    WNDPROC mStatusBarOriginProc = nullptr;

    bool mIsMainWindowSizeMoving = false;
    Rect mMainWindowRect;
    Rect mMainWindowRectForSizing;

    uint32_t mInitErrors = HacksInitErrors::NoError;
    DWORD mInstallHooksWin32Error = ERROR_SUCCESS;
};