#pragma once

#include "Contanst.h"
#include "Core/Constant.h"

#include <chrono>

namespace XSPlayer {

    class RightPannel : public DuiLib::CContainerUI
        , public DuiLib::INotifyUI {
        using supper = DuiLib::CContainerUI;
    public:
        RightPannel();
        ~RightPannel() override;

    public:
        void Notify(DuiLib::TNotifyUI& msg) override;

    public:
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

        void InitWindow(void);

    private:
        void Init(void);
        bool OnPlayMedia(void);
        bool OnPlayMedida(DuiLib::CControlUI* pControl);
        bool OnPlayLocalMedia(void);
        bool OnPlayServerMedia(void);

        bool PushToList(const std::vector<String>&& filePath);
        void OnAddListItem(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void OnBtnPlay(void);
        void InitPlaySolider(WPARAM wParam, LPARAM lParam);
        void UpdatePlayPos(WPARAM wParam, LPARAM lParam);
        void UpdateTimeText(void);

        void OnBtnNext();
        void OnBtnLast();
        void OnBtnStop();
        void OnSeekPostion();

    private:
        void ChangePlayImage(bool bPlay);
    private:
        enum class PlayStatus {
            PS_Stop,
            PS_Playing,
            PS_Puase
        };

        DuiLib::CSliderUI* m_pSliderPlayUI = nullptr;
        DuiLib::CLabelUI* m_pLabelPlayTime = nullptr;
        DuiLib::CLabelUI* m_pLabelTotoleTime = nullptr;
        DuiLib::CButtonUI* m_pBtnPlay = nullptr;
        int m_nDuration = 0;
        int m_nCurrentDuration = 0;
        PlayStatus m_playStatus = PlayStatus::PS_Stop;
        MediaSourceType m_mediaSoruce = MediaSourceType::MST_LOCAL;
        std::chrono::high_resolution_clock::time_point m_lastPlayTime;
        bool m_bSeek = false;
    };
}
