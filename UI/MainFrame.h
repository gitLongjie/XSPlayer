#pragma once

#include "Contanst.h"
#include "Core/Application.h"
#include "Script/PyEnvironment.h"
#include "Core/MediaManager.h"
#include "Core/Event.h"

namespace XSPlayer {

    class XSControlUI;
    class LrcEvent;
    class MediaTabControl;
    class RenderEvent;
    class UIEvent;

    class MainFrame : public DuiLib::WindowImplBase, public Application , public EventHandle{
        using supper = DuiLib::WindowImplBase;
    public:
        MainFrame();
        ~MainFrame() override;

        void InitWindow() override;
        void OnFinalMessage(HWND hWnd) override;
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
        void Notify(DuiLib::TNotifyUI& msg) override;
        DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass) override;

        bool OnNotify(const EventPtr& pEvent) override;

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

        bool OnUIEventNotify(UIEvent* uiEvent);
    private:
        bool OnControlEvent(const ControlEvent* pEvent);
        bool OnLrcEvent(const LrcEvent* pEvent);
        bool OnRenderEvent(const RenderEvent* pEvent);
        bool OnMediaTypeCreateEvent(const MediaSourceTypeCreateEvent* pEvent);
        bool OnAddMediaItem(const MediaSourceEvent* pEvent);

    private:
        void AddXSControl(XSControlUI* pControl);
        void RemoveXSControl(XSControlUI* pControl);
        void RemoveAllXSControl(void);

    private:
        using ListXSControl = std::vector<XSControlUI*>;
        ListXSControl m_listControl;

        NOTIFYICONDATA m_trayIcon = {};
        String m_curPlayTitle;
        PyEnvironment m_pyEnvironment;
    };
}
