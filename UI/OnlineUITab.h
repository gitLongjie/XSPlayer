#pragma once

#include "Contanst.h"
#include "Core/Constant.h"
#include "Core/MediaSource.h"

namespace XSPlayer {
    class MediaList;
    class MediaListItemContainer;
    class MediaSource9Ku;
    class MediaItem;

    class OnlineUITab : public DuiLib::CContainerUI
        , public DuiLib::INotifyUI
        , public MediaSourceCallback {
        using supper = DuiLib::CContainerUI;
    public:
        OnlineUITab(DuiLib::CPaintManagerUI* pPaintManagerUI);
        ~OnlineUITab() override;

    public:
        void DoEvent(DuiLib::TEventUI& event) override;
        void Notify(DuiLib::TNotifyUI& msg) override;

        void OnLoadedCallback(Media* pMedia) override;

    public:
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void InitWindow();

    private:
        void Init(DuiLib::CPaintManagerUI* pPaintManagerUI);

        bool PushToList(const std::vector<String>&& filePath);
        void OnAddListItem(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void NextMedia();

        void SortByNumber();

        void LastMedia();
        void StopMedia();
        void OnLoadMediaContent(MediaListItemContainer* pListItemContainer,
                                std::shared_ptr<MediaSource9Ku> pMediaSource,
                                MediaItem* mediaData);

        void OnAddItem(LPARAM args);

        void OnMediaItems(LPARAM lParam);
    private:
        MediaList* m_pMediaList = nullptr;
    };
}
