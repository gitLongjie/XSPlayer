#include "UI/OfflineUITab.h"

#include <string>
#include <functional>
#include <algorithm>

#include<Shlobj.h>

#include "Core/Constant.h"
#include "Core/MediaManager.h"
#include "Utils.h"

#include "UI/Menu.h"
#include "UI/MediaList.h"
#include "UI/ListItem.h"
#include "UI/BuildCallback.h"

namespace XSPlayer {

    OfflineUITab::OfflineUITab(DuiLib::CPaintManagerUI* pPaintManagerUI) : supper() {
        Init(pPaintManagerUI);
    }

    OfflineUITab::~OfflineUITab() {
        
    }

    void OfflineUITab::DoEvent(DuiLib::TEventUI& event) {
        if (event.Type == DuiLib::UIEVENT_BUTTONDOWN) {
            return;
        }
        supper::DoEvent(event);
    }

    void OfflineUITab::Notify(DuiLib::TNotifyUI& msg) {
        if (_tcsicmp(msg.sType, kClick) == 0) {
            DuiLib::CDuiString strName = msg.pSender->GetName();
            if (0 == _tcsicmp(strName, kbtnLoadMedia)) {
                PostMessage(m_pManager->GetPaintWindow(), WM_OFFLINE_LOADMEIDA, 0, 0);
            }
            else if (0 == _tcsicmp(strName, kbtnLoadMediaFolder)) {
                LoadLocalMediaFolder();
            }
        }
        else if (0 == _tcsicmp(msg.sType, _T("menu")))
        {
            if (msg.pSender->GetName() != kOfflineList) return;
            MenuWnd* pMenu = new MenuWnd();
            if (pMenu == NULL) { return; }
            POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
            ::ClientToScreen(m_pManager->GetPaintWindow(), &pt);
            pMenu->Init(msg.pSender, pt);
        }
        else if (0 == _tcsicmp(msg.sType, kMenuLoadMedia)) {
            PostMessage(m_pManager->GetPaintWindow(), WM_OFFLINE_LOADMEIDA, 0, 0);
        }
        else if (0 == _tcsicmp(msg.sType, kMenuDeleteMedia)) {
            DeleteLocalMedia();
        }
        else if (0 == _tcsicmp(msg.sType, _T("menu_Sort_Number"))) {
            SortByNumber();
        }

        else if (msg.sType == _T("itemactivate")) {
            auto pListItem = reinterpret_cast<ListItem*>(msg.pSender->GetTag());
            if (nullptr == pListItem) {
                return;
            }
            DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOfflineList));
            if (nullptr == pList || pListItem->GetMediaList() != pList) {
                return;
            }
            int nCurSel = pList->GetCurSel();
            PostMessage(m_pManager->GetPaintWindow(), WM_OFFLINE_PLAY, nCurSel, 0);
        }
    }

    void OfflineUITab::OnLoadedCallback(Media* pMedia) {
        if (nullptr == pMedia) {
            return;
        }
        
        auto pMeidaListItem = new MediaListItem(pMedia->GetText());
        pMeidaListItem->SetMediaID(pMedia->GetMediaId());
        pMeidaListItem->SetMediaPath(pMedia->GetMediaPath());
        PostMessage(m_pManager->GetPaintWindow(), WM_OFFLINE_ADD_LISTITEM, 0, (LPARAM)pMeidaListItem);
    }

    LRESULT OfflineUITab::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg)
        {
        case WM_OFFLINE_ADD_LISTITEM:
            OnAddListItem(uMsg, wParam, lParam);
            break;

        case WM_OFFLINE_LOADMEIDA: {
            std::vector<String> filePath;
            LoadLocalMedia(filePath);
            PushToList(std::move(filePath));
        } break;

        case WM_OFFLINE_NEXT:
        case WM_CHANGE_NEXT_PLAY:
        {
            NextMedia();
        }break;

        case WM_CHANGE_LAST_PLAY:
        {
            LastMedia();
        }break;

        case WM_CHANGE_STOP_PLAY:
        {
            StopMedia();
        }break;


        default:
            break;
        }

        return 0;
    }

    void OfflineUITab::InitWindow() {
//         DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOfflineList));
//         if (nullptr == pList) {
//             return;
//         }
// 
//         auto pMediaList = dynamic_cast<MediaList*>(pList);
//         if (nullptr == pMediaList) {
//             return;            
//         }
// 
//         int nCount = MediaManager::GetSingleton().GetMediaCount();
//         for (int index = 0; index < nCount; ++index) {
//             const String name = MediaManager::GetSingleton().GetLocalMedia(index);
//             auto pMediaListItem = new MediaListItem(name.c_str());
//             pMediaListItem->SetText(name.c_str());
//             pMediaListItem->SetMediaArtist(_T("未知"));
//             pMediaList->Add(pMediaListItem);
//         }
// 
//         int curPlay = MediaManager::GetSingleton().GetCurrentPlay();
// 
//         if (nCount >= 0) {
//             if (curPlay != -1) {
//                 pList->SelectItem(curPlay);
//                 return;
//             }
//             pList->SelectItem(0);
//         }
    }

    void OfflineUITab::Init(DuiLib::CPaintManagerUI* pPaintManagerUI) {
        DuiLib::CDialogBuilder dailogBuilder;
        BuildCallback callback(pPaintManagerUI);
        DuiLib::CContainerUI* pContainerUI = dynamic_cast<DuiLib::CContainerUI*>(dailogBuilder.Create(_T("Data/skin/chinesestyle/offline_tab.xml"), (UINT)0, &callback));
        if (nullptr != pContainerUI) {
            Add(pContainerUI);
        }
        else {
            RemoveAll();
        }
    }

    void OfflineUITab::LoadLocalMedia(std::vector<String>& filePaths) const {
        TCHAR szOpenFileNames[MAX_PATH * MAX_PATH] = {0};
        TCHAR szPath[MAX_PATH];
        static TCHAR szFilter[] = TEXT("MP3 Files (*.mp3)\0*.mp3\0") \
            TEXT("All Files (*.*)\0*.*\0\0");

        OPENFILENAME  ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lpstrFile = szOpenFileNames;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szOpenFileNames);
        ofn.lpstrFilter = szFilter;
        ofn.lpstrTitle = NULL;
        ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
        ofn.lStructSize = sizeof(OPENFILENAME);  // No OFN_ENABLEHOOK
        ofn.hwndOwner = m_pManager->GetPaintWindow();
        if (GetOpenFileName(&ofn)) {
            lstrcpyn(szPath, szOpenFileNames, ofn.nFileOffset);
            szPath[ofn.nFileOffset] = '\0';
            int nLen = lstrlen(szPath);

            if (szPath[nLen - 1] != '/')   //如果选了多个文件,则必须加上'//'
            {
                lstrcat(szPath, TEXT("/"));
            }

            TCHAR* pszFileName = szOpenFileNames + ofn.nFileOffset; //把指针移到第一个文件
            while (*pszFileName)
            {
                String strefileName(szPath);
                strefileName.append(pszFileName);
                pszFileName += lstrlen(pszFileName) + 1;
                filePaths.emplace_back(strefileName);
            }
        }
    }

    void OfflineUITab::LoadLocalMediaFolder() {
        TCHAR szBuffer[MAX_PATH] = { 0 };
        BROWSEINFO bi = { 0 };
        bi.hwndOwner = m_pManager->GetPaintWindow();
        bi.pszDisplayName = szBuffer;
        bi.lpszTitle = _T("选择一个文件夹");
        bi.ulFlags = BIF_NEWDIALOGSTYLE;
        LPITEMIDLIST idl = SHBrowseForFolder(&bi);
        if (SHGetPathFromIDList(idl, szBuffer)) {
            MessageBox(NULL, szBuffer, TEXT("你选择的文件夹"), 0);
        }
        
    }

    void OfflineUITab::DeleteLocalMedia() {
        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOfflineList));
        if (nullptr == pList) {
            return;
        }
        int nSel = pList->GetCurSel();
        if (-1 == nSel) {
            return;
        }

        int curPlay = MediaManager::GetSingleton().GetCurrentPlay();
        if (curPlay == nSel) {
            MediaManager::GetSingleton().Stop();
        }
        MediaManager::GetSingleton().DeleteMedia(nSel);
        pList->RemoveAt(nSel);
    }

    bool OfflineUITab::PushToList(const std::vector<String>&& filePath) {
        if (filePath.empty()) {
            return false;
        }

        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOfflineList));
        if (nullptr == pList) {
            return false;
        }
        auto pMediaList = dynamic_cast<MediaList*>(pList);
        if (nullptr == pMediaList) {
            return false;
        }
        
        for (const auto& item : filePath) {
            String name;
            if (!Utils::GetFileNameByPath(item, name)) {
                continue;
            }
            
            auto pMediaItem = new MediaItem(name);
            pMediaItem->SetMediaPath(item);
            MediaManager::GetSingleton().AddMedia(pMediaItem, kOfflineSource);
        }

        return true;

    }

    void OfflineUITab::OnAddListItem(UINT uMsg, WPARAM wParam, LPARAM lParam)  {
        MediaList* pList = static_cast<MediaList*>(m_pManager->FindControl(kOfflineList));
        if (nullptr == pList) {
            return;
        }
        MediaListItem* pListElement = (MediaListItem*)(lParam);
        pList->Add(pListElement);
    }

    void OfflineUITab::NextMedia() {
        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOfflineList));
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
        PostMessage(m_pManager->GetPaintWindow(), WM_OFFLINE_PLAY, nCur, 0);
    }

    void OfflineUITab::SortByNumber() {
        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOfflineList));
        if (nullptr == pList) {
            return;
        }

        pList->RemoveAll();
        MediaManager::GetSingleton().SortLocalMediaByNumberName();
    }

    void OfflineUITab::LastMedia() {
        DuiLib::CListUI* pList = static_cast<DuiLib::CListUI*>(m_pManager->FindControl(kOfflineList));
        if (nullptr == pList) {
            return;
        }

        int nCur = pList->GetCurSel();
        if (nCur <= 0) {
            return;
        }
        nCur -= 1;
        pList->SelectItem(nCur);
        PostMessage(m_pManager->GetPaintWindow(), WM_OFFLINE_PLAY, nCur, 0);
    }

    void OfflineUITab::StopMedia() {
    }

}