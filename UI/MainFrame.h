#pragma once

#include "Contanst.h"
#include "Core/Application.h"
#include "Script/PyEnvironment.h"

namespace XSPlayer {

    class OfflineUITab;
    class OnlineUITab;
    class RightPannel;

    class MainFrame : public DuiLib::WindowImplBase, public Application {
        using supper = DuiLib::WindowImplBase;
    public:
        MainFrame();
        ~MainFrame() override;

        void InitWindow() override;
        void OnFinalMessage(HWND hWnd) override;
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
        void Notify(DuiLib::TNotifyUI& msg) override;
        DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass) override;

        void Init(void) override;
        UINT GetClassStyle() const override;

    protected:
        DuiLib::UILIB_RESOURCETYPE GetResourceType() const override;
        DuiLib::CDuiString GetSkinFolder() override;
        DuiLib::CDuiString GetSkinFile() override;
        LPCTSTR GetWindowClassName(void) const override;

    protected:
        void OnExit(DuiLib::TNotifyUI& msg);
        void OnSelectChanged(DuiLib::TNotifyUI& msg);

    private:
        void AddTrayIcon();
        LRESULT OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        void OnInitMediaManager(void);

    private:
        OfflineUITab* m_pOfflineUITab = nullptr;
        OnlineUITab* m_pOnlineUITab = nullptr;
        RightPannel* m_pRightPannel = nullptr;

        NOTIFYICONDATA m_trayIcon;
        String m_curPlayTitle;
        PyEnvironment m_pyEnvironment;
    };
}
