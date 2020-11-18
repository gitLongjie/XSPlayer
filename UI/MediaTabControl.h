#pragma once

#include "Contanst.h"
#include "Core/Constant.h"
#include "UI/XSControlUI.h"

namespace XSPlayer {
    class OfflineUITab;
    class Media;

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

    private:
        bool NotifyOption(DuiLib::TNotifyUI& msg);
        bool OnAddMediaTypeTab(WPARAM wParam, LPARAM lParam);
        bool OnAddMedia(WPARAM wParam, LPARAM lParam);
        bool OnAddToMediaList(Media* pMedia);
        void OnPlay(unsigned int id);
        bool OnPlayNext();
        bool OnPlayLast();

    private:
        static DuiLib::CDialogBuilder m_optionUIBuilder;
        String m_normalImage;
        String m_hotImage;
        String m_pushedImage;
        String m_group;
        DuiLib::CVerticalLayoutUI* m_pTabHeader = nullptr;
        OfflineUITab* m_pMediaList = nullptr;
        String m_curMediaSource;
    };
}
