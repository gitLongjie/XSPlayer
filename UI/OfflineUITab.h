#pragma once

#include "Contanst.h"
#include "Core/Constant.h"
#include "Core/MediaSource.h"

namespace XSPlayer {

    class OfflineUITab : public DuiLib::CContainerUI
        , public DuiLib::INotifyUI
        , public MediaSourceCallback {
        using supper = DuiLib::CContainerUI;
    public:
        OfflineUITab(DuiLib::CPaintManagerUI* pPaintManagerUI);
        ~OfflineUITab() override;

    public:
        void DoEvent(DuiLib::TEventUI& event) override;
        void Notify(DuiLib::TNotifyUI& msg) override;

        void OnLoadedCallback(Media* pMedia) override;

    public:
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void InitWindow();

    private:
        void Init(DuiLib::CPaintManagerUI* pPaintManagerUI);
        void LoadLocalMedia(std::vector<String>& filePath) const;
        void LoadLocalMediaFolder();
        void DeleteLocalMedia();

        bool PushToList(const std::vector<String>&& filePath);
        void OnAddListItem(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void NextMedia();

        void SortByNumber();

        void LastMedia(const MediaSourceType sourceType);
        void StopMedia();
    };
}
