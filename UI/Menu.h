#pragma once

#include "Contanst.h"

namespace XSPlayer {

    class MenuWnd : public DuiLib::CWindowWnd, public DuiLib::INotifyUI {
        using supper = DuiLib::CWindowWnd;
    public:
        MenuWnd();
        ~MenuWnd();

    public:
        void Notify(DuiLib::TNotifyUI& msg) override;

        void Init(DuiLib::CControlUI* pOwner, POINT pt);

    protected:
        LPCTSTR GetWindowClassName() const override { return _T("MenuWnd"); }
        void OnFinalMessage(HWND /*hWnd*/) override { delete this; }
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    private:
        void AdjustPostion(void);
        HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle,
            DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
            int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
        void ShowWindow(bool bShow = true, bool bTakeFocus = true);
        LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    private:
        DuiLib::CPaintManagerUI m_pm;
        DuiLib::CControlUI* m_pOwner;
        POINT m_ptPos;

    };
}
