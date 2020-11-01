#pragma once

#include "Contanst.h"
#include "Core/Constant.h"
#include "UI/XSControlUI.h"

namespace XSPlayer {

    class MediaTabControl : public XSControlUI {
    public:
        explicit MediaTabControl(void);
        ~MediaTabControl() override;


    public:
        void DoInit(void) override;
        void InitWindow(void) override;
        void UnInitWindow(void) override;
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
        void Notify(DuiLib::TNotifyUI& msg) override;
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

        bool AddTab(const String& txt, const String& mediaSource);

    protected:
        DuiLib::COptionUI* CreateTabUI(void);
        DuiLib::CControlUI* CreateTabBody(const String& mediaSource);

    private:
        bool NotifyOption(DuiLib::TNotifyUI& msg);
        bool OnNotifyOption(DuiLib::COptionUI* pOptionUI);
        bool OnAddMediaTypeTab(WPARAM wParam, LPARAM lParam);
        bool OnAddList(WPARAM wParam, LPARAM lParam);
        void OnPlay(unsigned int id);

    private:
        static DuiLib::CDialogBuilder m_optionUIBuilder;
        String m_normalImage;
        String m_hotImage;
        String m_pushedImage;
        String m_group;
        DuiLib::CVerticalLayoutUI* m_pTabHeader = nullptr;
        DuiLib::CTabLayoutUI* m_pTabBody = nullptr;
    };
}
