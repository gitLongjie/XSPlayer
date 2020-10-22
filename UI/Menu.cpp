#include "UI/Menu.h"

#include <string>

#include<Shlobj.h>

#include "Contanst.h"

namespace XSPlayer {

    MenuWnd::MenuWnd() : supper() {
    }

    MenuWnd::~MenuWnd() {
        
    }


    void MenuWnd::Init(DuiLib::CControlUI* pOwner, POINT pt) {
        if (pOwner == NULL) return;
        m_pOwner = pOwner;
        m_ptPos = pt;
        Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_VISIBLE | 
            WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
        HWND hWndParent = m_hWnd;
        while (::GetParent(hWndParent) != NULL) hWndParent = ::GetParent(hWndParent);
        ::ShowWindow(m_hWnd, SW_SHOW);
        ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
    }

    HWND MenuWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle,
        DWORD dwExStyle, int x /*= CW_USEDEFAULT*/, int y /*= CW_USEDEFAULT*/,
        int cx /*= CW_USEDEFAULT*/, int cy /*= CW_USEDEFAULT*/, HMENU hMenu /*= NULL*/) {
        
        dwExStyle |= WS_EX_TOOLWINDOW;
        return supper::Create(hwndParent, pstrName, dwStyle, dwExStyle, x, y, cx, cy, hMenu);
    }

    void MenuWnd::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= true*/) {
        supper::ShowWindow(bShow, bTakeFocus);
    }

    LRESULT MenuWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        m_pm.Init(m_hWnd);
        DuiLib::CDialogBuilder builder;
        DuiLib::CControlUI* pRoot = builder.Create(_T("Data/skin/chinesestyle/menu.xml"), (UINT)0, NULL, &m_pm);
        ASSERT(pRoot && "Failed to parse XML");
        m_pm.AttachDialog(pRoot);
        m_pm.AddNotifier(this);
        m_pm.SetRoundCorner(3, 3);

        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pOwner);
        int nSel = pList->GetCurSel();
        if (nSel < 0) {
            DuiLib::CControlUI* pControl = m_pm.FindControl(_T("menu_Delete"));
            if (pControl) pControl->SetEnabled(false);
        }

        AdjustPostion();
        return 0;
    }

    LRESULT MenuWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        bHandled = FALSE;
        return 0;
    }

    LRESULT MenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        if ((HWND)wParam == m_hWnd) bHandled = TRUE;
        else {
            Close();
            bHandled = FALSE;
        }
        return 0;
    }

    LRESULT MenuWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        if (wParam == VK_ESCAPE) Close();
        return 0;
    }

    LRESULT MenuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        if (!::IsIconic(*this)) {
            DuiLib::CDuiRect rcWnd;
            ::GetWindowRect(*this, &rcWnd);
            rcWnd.Offset(-rcWnd.left, -rcWnd.top);
            HRGN hRgn = ::CreateRectRgn(rcWnd.left + 8, rcWnd.top + 8, rcWnd.right - 8, rcWnd.bottom - 8);
            ::SetWindowRgn(*this, hRgn, TRUE);
            ::DeleteObject(hRgn);
        }
        
        bHandled = FALSE;
        return 0;
    }

    LRESULT MenuWnd::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        bHandled = FALSE;
        return 0;
    }

    LRESULT MenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;
        switch (uMsg) {
        case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
        case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
        case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
        case WM_KEYDOWN:       lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
        case WM_MOUSEWHEEL:    break;
        case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
        case WM_MOVE:          lRes = OnMove(uMsg, wParam, lParam, bHandled); break;
        default:
            bHandled = FALSE;
        }
        if (bHandled) return lRes;
        if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

    void MenuWnd::AdjustPostion(void) {
        DuiLib::CDuiRect rcWnd;
        GetWindowRect(m_hWnd, &rcWnd);
        int nWidth = rcWnd.GetWidth();
        int nHeight = rcWnd.GetHeight();
        rcWnd.left = m_ptPos.x;
        rcWnd.top = m_ptPos.y;
        rcWnd.right = rcWnd.left + nWidth;
        rcWnd.bottom = rcWnd.top + nHeight;
        MONITORINFO oMonitor = {};
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
        DuiLib::CDuiRect rcWork = oMonitor.rcWork;

        if (rcWnd.bottom > rcWork.bottom) {
            if (nHeight >= rcWork.GetHeight()) {
                rcWnd.top = 0;
                rcWnd.bottom = nHeight;
            }
            else {
                rcWnd.bottom = rcWork.bottom;
                rcWnd.top = rcWnd.bottom - nHeight;
            }
        }
        if (rcWnd.right > rcWork.right) {
            if (nWidth >= rcWork.GetWidth()) {
                rcWnd.left = 0;
                rcWnd.right = nWidth;
            }
            else {
                rcWnd.right = rcWork.right;
                rcWnd.left = rcWnd.right - nWidth;
            }
        }
        ::SetWindowPos(m_hWnd, NULL, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    void MenuWnd::Notify(DuiLib::TNotifyUI& msg) {
        if (msg.sType == _T("itemselect")) {
            Close();
        }
        else if (msg.sType == _T("itemclick")) {
            DuiLib::CDuiString strName = msg.pSender->GetName();
            if (msg.pSender->GetName() == _T("menu_Delete")) {
                if (m_pOwner) m_pOwner->GetManager()->SendNotify(m_pOwner, _T("menu_Delete"), 0, 0, true);
            }
            else if (0 == _tcsicmp(strName, kMenuLoadMedia)) {
                if (nullptr != m_pOwner) {
                    m_pOwner->GetManager()->SendNotify(m_pOwner, kMenuLoadMedia, 0, 0, true);
                }
            }
            else if (0 == _tcsicmp(strName, _T("menu_Sort_Number"))) {
                if (nullptr != m_pOwner) {
                    m_pOwner->GetManager()->SendNotify(m_pOwner,  _T("menu_Sort_Number"), 0, 0, true);
                }
            }
        }
    }

}