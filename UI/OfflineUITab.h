#pragma once

#include "Contanst.h"
#include "Core/Constant.h"
#include "Core/MediaSource.h"

#include "UI/XSControlUI.h"

namespace XSPlayer {
    class MediaList;

    class OfflineUITab : public XSControlUI {
    public:
        explicit OfflineUITab(void);
        ~OfflineUITab() override;

    public:
        void DoEvent(DuiLib::TEventUI& event) override;
        void Notify(DuiLib::TNotifyUI& msg) override;

    public:
        void DoInit(void) override;
        void InitWindow(void) override;

        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

        void AddMedia(const Media* pMedia);
        void RemoveAllMedia(void);
        bool PlayNext(void);
        bool PlayLast(void);

    private:
        
        void LoadLocalMedia(std::vector<String>& filePath) const;
        void LoadLocalMediaFolder();
        void DeleteLocalMedia();

        bool PushToList(const std::vector<String>&& filePath);
        void OnAddListItem(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void NextMedia(void);

        void SortByNumber();

        void LastMedia(void);

    private:
        MediaList* m_pMediaList = nullptr;
    };
}
