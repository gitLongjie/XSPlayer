#include "UI/OnlineUITab.h"

#include <string>
#include <functional>
#include <algorithm>

#include<Shlobj.h>

#include "Core/MediaManager.h"
#include "Core/ThreadPool.h"
#include "Core/MediaItem.h"

#include "Script/MediaSource9Ku.h"

#include "UI/Menu.h"
#include "UI/MediaList.h"
#include "UI/ListItem.h"

namespace XSPlayer {

    class OfflineTabBuildCallback : public DuiLib::IDialogBuilderCallback {
    public:
        OfflineTabBuildCallback(DuiLib::CPaintManagerUI* pPaintManagerUI) : m_pPaintManagerUI(pPaintManagerUI) {

        }

        DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass) {
            if (0 == _tcsicmp(pstrClass, kMediaListUI)) {
                return new MediaList(m_pPaintManagerUI);
            }
            return nullptr;
        }

    private:
        DuiLib::CPaintManagerUI* m_pPaintManagerUI;
    };

    OnlineUITab::OnlineUITab(DuiLib::CPaintManagerUI* pPaintManagerUI) : supper() {
        Init(pPaintManagerUI);
    }

    OnlineUITab::~OnlineUITab() {
        
    }

    void OnlineUITab::DoEvent(DuiLib::TEventUI& event) {
        if (event.Type == DuiLib::UIEVENT_BUTTONDOWN) {
            return;
        }
        supper::DoEvent(event);
    }

    void OnlineUITab::Notify(DuiLib::TNotifyUI& msg) {
        if (_tcsicmp(msg.sType, kClick) == 0) {
           
        }
        else if (0 == _tcsicmp(msg.sType, _T("menu")))
        {
//             if (msg.pSender->GetName() != _T("domainlist")) return;
//             MenuWnd* pMenu = new MenuWnd();
//             if (pMenu == NULL) { return; }
//             POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
//             ::ClientToScreen(m_pManager->GetPaintWindow(), &pt);
//             pMenu->Init(msg.pSender, pt);
        }
        else if (0 == _tcsicmp(msg.sType, kMenuLoadMedia)) {
//          PostMessage(m_pManager->GetPaintWindow(), WM_OFFLINE_LOADMEIDA, 0, 0);
//             std::vector<String> filePath;
//             LoadLocalMedia(filePath);
//             PushToList(std::move(filePath));
        }
        else if (0 == _tcsicmp(msg.sType, kMenuDeleteMedia)) {
           
        }
        else if (0 == _tcsicmp(msg.sType, _T("menu_Sort_Number"))) {
            
        }

        else if (msg.sType == _T("itemactivate")) {
            auto pListItem = reinterpret_cast<ListItem*>(msg.pSender->GetTag());
            if (nullptr == pListItem) {
                return;
            }
            DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOnlineList));
            if (nullptr == pList || pListItem->GetMediaList() != pList) {
                return;
            }
            int nCurSel = pList->GetCurSel();
            PostMessage(m_pManager->GetPaintWindow(), WM_ONLINE_PLAY, nCurSel, 0);
        }
    }

    void OnlineUITab::OnLoadedCallback(Media* pMedia) {
        if (nullptr == pMedia) {
            return;
        }

        if (!pMedia->IsLeaf()) {
            auto pMeidaListContainer = new MediaListItemContainer(pMedia->GetText());
            pMeidaListContainer->SetMediaID(pMedia->GetMediaId());
            pMeidaListContainer->SetMediaPath(pMedia->GetMediaPath());
            PostMessage(m_pManager->GetPaintWindow(), WM_ONLINE_ADD_LISTITEM, 0, (LPARAM)pMeidaListContainer);
        }
        else {
            auto pMediaItem = dynamic_cast<MediaItem*>(pMedia);
            if (nullptr == pMediaItem) {
                return;
            }
            PostMessage(m_pManager->GetPaintWindow(), WM_ONLINE_ADD_LISTITEM, 0, (LPARAM)pMediaItem);
        }
        
    }

    LRESULT OnlineUITab::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_ONLINE_ADD_LISTITEM:
            OnAddItem(lParam);
            break;

        case WM_ONLINE_ADD_MEDIAITEM:
            OnMediaItems(lParam);
            break;

        case WM_ONLINE_NEXT:
            NextMedia();
            break;

        default:
            break;
        }
        return 0;
    }

    void OnlineUITab::InitWindow() {
        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOnlineList));
        if (nullptr == pList) {
            return;
        }

        m_pMediaList = dynamic_cast<MediaList*>(pList);
    }

    void OnlineUITab::Init(DuiLib::CPaintManagerUI* pPaintManagerUI) {
        DuiLib::CDialogBuilder dailogBuilder;
        OfflineTabBuildCallback callback(pPaintManagerUI);
        DuiLib::CContainerUI* pContainerUI = dynamic_cast<DuiLib::CContainerUI*>(dailogBuilder.Create(_T("Data/skin/chinesestyle/online_tab.xml"), (UINT)0, &callback));
        if (nullptr != pContainerUI) {
            Add(pContainerUI);
        }
        else {
            RemoveAll();
        }
    }

    bool OnlineUITab::PushToList(const std::vector<String>&& filePath) {
        if (filePath.empty()) {
            return false;
        }

        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOnlineList));
        if (nullptr == pList) {
            return false;
        }
        auto pMediaList = dynamic_cast<MediaList*>(pList);
        if (nullptr == pMediaList) {
            return false;
        }
        
        for (const auto& item : filePath) {
            String name;
//             if (MediaManager::GetSingleton().AddMedia(item)) {
//                 name = MediaManager::GetSingleton().GetLocalMedia(MediaManager::GetSingleton().GetMediaFileCount() - 1);
//                 auto pMediaListItem = new MediaListItem(name.c_str());
// 
//                 pMediaListItem->SetMediaArtist(_T("未知"));
//                 pMediaList->Add(pMediaListItem);
//             }
        }

        return true;

    }

    void OnlineUITab::OnAddListItem(UINT uMsg, WPARAM wParam, LPARAM lParam)  {
        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOnlineList));
        if (nullptr == pList) {
            return;
        }
        DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)(lParam);
        pList->Add(pListElement);
    }

    void OnlineUITab::NextMedia() {
        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOnlineList));
        if (nullptr == pList) {
            return;
        }

        int nCount = pList->GetCount();
        int nCur = pList->GetCurSel();
        if (nCur >= nCount) {
            return;
        }
        nCur += 1;
        pList->SelectItem(nCur);

        DuiLib::CControlUI* pListElement = pList->GetItemAt(nCur);
        if (nullptr == pListElement) {
            return;
        }

        ListItem* pListItem = reinterpret_cast<ListItem*>(pListElement->GetTag());
        if (nullptr == pListItem) {
            return;
        }

        if (pListItem->HasChildren()) {
            NextMedia();
            return;
        }

        PostMessage(m_pManager->GetPaintWindow(), WM_ONLINE_PLAY, nCur, 0);
    }

    void OnlineUITab::SortByNumber() {
        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOnlineList));
        if (nullptr == pList) {
            return;
        }

        pList->RemoveAll();
        MediaManager::GetSingleton().SortLocalMediaByNumberName();
    }

    void OnlineUITab::LastMedia() {
        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOnlineList));
        if (nullptr == pList) {
            return;
        }

        int nCur = pList->GetCurSel();
        if (nCur <= 0) {
            return;
        }
        nCur -= 1;
        pList->SelectItem(nCur);
    }

    void OnlineUITab::StopMedia() {
    }

    void OnlineUITab::OnLoadMediaContent(MediaListItemContainer* pListItemContainer,
                                         std::shared_ptr<MediaSource9Ku> pMediaSource,
                                         MediaItem* mediaData) {
        if (nullptr == pListItemContainer || nullptr == pMediaSource) {
            return;
        }
// 
//         MediaDatas netMediaItems = std::move(pMediaSource->BuilderMediaByType(mediaData));
//         auto* pArgs = new Args<MediaDatas, MediaListItemContainer*>(std::move(netMediaItems), pListItemContainer);
//         PostMessage(m_pManager->GetPaintWindow(), WM_ONLINE_ADD_MEDIAITEM, 0, (LPARAM)pArgs);
    }

    void OnlineUITab::OnAddItem(LPARAM args) {
        if (nullptr == m_pMediaList) {
            return;
        }

        Media* pMedia = (Media*)args;
        if (nullptr == pMedia) {
            return;
        }

        if (!pMedia->IsLeaf()) {
            m_pMediaList->Add(dynamic_cast<MediaListItemContainer*>(pMedia));
        }
        else {
            auto pMediaItem = dynamic_cast<MediaItem*>(pMedia);
            auto pMeidaListItem = new MediaListItem(pMediaItem->GetText());
            pMeidaListItem->SetMediaID(pMedia->GetMediaId());
            pMeidaListItem->SetMediaPath(pMedia->GetMediaPath());
            pMeidaListItem->SetMediaArtist(pMediaItem->GetAirt());

            Media* pParentMedia = pMediaItem->GetParent();
            if (nullptr == pParentMedia) {
                delete pMeidaListItem;
                return;
            }

            const String& text = pParentMedia->GetText();
            Media* pFindMedia = m_pMediaList->Find(text);
            if (nullptr == pFindMedia) {
                delete pMeidaListItem;
                return;
            }

            auto pMeidaListItemContainer = dynamic_cast<MediaListItemContainer*>(pFindMedia);
            if (nullptr == pMeidaListItemContainer) {
                delete pMeidaListItem;
                return;
            }

            pMeidaListItemContainer->Add(pMeidaListItem);
        }
    }

    void OnlineUITab::OnMediaItems(LPARAM lParam) {
        auto pArgs = reinterpret_cast<Args<MediaDatas, MediaListItemContainer*>*>(lParam);
        if (nullptr == pArgs) {
            return;
        }

        MediaDatas mediaItems = std::move(pArgs->GetData());
        MediaListItemContainer* pMediaListItem = pArgs->GetContext();
        delete pArgs;

        if (nullptr == pMediaListItem) {
            return;
        }

        for (const auto& item : mediaItems) {
            auto pMediaItem = new MediaListItem(item->GetText());
            pMediaItem->SetMediaArtist(item->GetAirt());
            pMediaItem->SetMediaPath(item->GetMediaPath());
            pMediaListItem->Add(pMediaItem);
        }
    }

}