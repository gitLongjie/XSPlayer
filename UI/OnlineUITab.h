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

        void OnAddListItem(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void NextMedia(const MediaSourceType sourceType);

        void SortByNumber();

        void LastMedia(const MediaSourceType sourceType);
        void StopMedia();

        void OnAddItem(LPARAM args);

        void OnMediaItems(LPARAM lParam);

        bool IsContainter(int index);
        
    private:
        MediaList* m_pMediaList = nullptr;
        int m_activeItem = -1;
    };
}
