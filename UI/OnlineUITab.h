#pragma once

#include "Contanst.h"
#include "Core/Constant.h"
#include "Core/MediaSource.h"

#include "UI/XSControlUI.h"

namespace XSPlayer {
    class MediaList;
    class MediaListItemContainer;
    class MediaSource9Ku;
    class MediaItem;

    class OnlineUITab : public XSControlUI {
    public:
        explicit OnlineUITab(void);
        ~OnlineUITab() override;

    public:
        void DoEvent(DuiLib::TEventUI& event) override;
        void Notify(DuiLib::TNotifyUI& msg) override;

    public:
        void DoInit(void) override;
        void InitWindow(void) override;

        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
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
