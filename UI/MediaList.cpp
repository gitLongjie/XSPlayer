#include "UI/MediaList.h"

#include "UI/ListItem.h"
#include <DuiLib/UIlib.h>


namespace XSPlayer {
    DuiLib::CDialogBuilder MediaList::m_listElementBuilder;

    MediaList::MediaList(DuiLib::CPaintManagerUI* pPaintManagerUI)
        : DuiLib::CListUI()
        , m_pPaintManagerUI(pPaintManagerUI)
        , m_pRoot(new MediaContainer()) {

    }

    MediaList::~MediaList() {
        if (nullptr != m_pRoot) {
            m_pRoot->Clear();
            delete m_pRoot;
        }
    }

    bool MediaList::Add(CControlUI* pControl) {
        if (nullptr == pControl) {
            return false;
        }

        if (_tcsicmp(pControl->GetClass(), DUI_CTR_LISTCONTAINERELEMENT) != 0) {
            return false;
        }

        return __super::Add(pControl);
    }

    bool MediaList::Add(ListItem* pListItem) {
        if (nullptr == pListItem || !pListItem->Init(this)) {
            return false;
        }

        DuiLib::CListContainerElementUI* pListElementUI = pListItem->GetListElement();
        if (nullptr == pListElementUI) {
            return false;
        }

        if (!__super::Add(pListElementUI)) {
            return false;
        }

        return pListItem->SetMediaParent(m_pRoot);
    }

    bool MediaList::Remove(CControlUI* pControl, bool bDoNotDestroy /*= false*/) {
        if (nullptr == pControl) {
            return false;
        }

        if (0 != _tcsicmp(pControl->GetClass(), DUI_CTR_LISTCONTAINERELEMENT)) {
            return false;
        }

        auto pListElement = static_cast<DuiLib::CListContainerElementUI*>(pControl->GetInterface(DUI_CTR_LISTCONTAINERELEMENT));
        if (nullptr != pListElement) {
            auto pListItem = reinterpret_cast<ListItem*>(pListElement);
            if (nullptr != pListElement) {
                return Remove(pListElement, !bDoNotDestroy);
            }
        }
        else {
            return __super::Remove(pControl, bDoNotDestroy);
        }
        return false;
    }

    bool MediaList::Remove(ListItem* pListItem, bool bDoNotDestroy /*= false*/) {
        if (nullptr == pListItem) {
            return false;
        }

        DuiLib::CListContainerElementUI* pListElementUI = pListItem->GetListElement();
        if (nullptr != pListItem) {
            Remove(pListElementUI);
        }

        auto pMedia = dynamic_cast<Media*>(pListElementUI);
        return m_pRoot->Remove(pMedia, bDoNotDestroy);
    }

    bool MediaList::SelectItem(int iIndex, bool bTakeFocus /*= false*/, bool bTriggerEvent /*= true*/) {
        int nSel = GetCurSel();
        if (nSel == iIndex) {
            return true;
        }

        if (!__super::SelectItem(iIndex, bTakeFocus, bTriggerEvent)) {
            return false;
        }

        UpdateStatus(nSel);
        UpdateStatus(GetCurSel());
        return true;
    }

    Media* MediaList::Find(const String& text) const {
        if (nullptr == m_pRoot) {
            return nullptr;
        }

        return m_pRoot->Find(text);
    }

    void MediaList::Clear(void) {
        if (nullptr != m_pRoot) {
            m_pRoot->Clear();
        }
    }

    DuiLib::CListContainerElementUI* MediaList::CreateElementUI() const {
        if (!m_listElementBuilder.GetMarkup()->IsValid()) {
            return static_cast<DuiLib::CListContainerElementUI*>(m_listElementBuilder.Create(_T("Data/skin/chinesestyle/media_list_item.xml"), (UINT)0, NULL, m_pPaintManagerUI));
        }
        else {
            return static_cast<DuiLib::CListContainerElementUI*>(m_listElementBuilder.Create((UINT)0, m_pPaintManagerUI));
        }
    }

    void MediaList::UpdateStatus(int index) {
        DuiLib::CControlUI* pControl = GetItemAt(index);
        if (nullptr == pControl) {
            return;
        }

        auto pListItem = reinterpret_cast<ListItem*>(pControl->GetTag());
        if (nullptr == pListItem) {
            return;
        }

        auto pMediaItem = dynamic_cast<MediaListItem*>(pListItem);
        if (nullptr == pMediaItem) {
            return;
        }

        pMediaItem->SetPlaying(GetCurSel() == index);
    }

    bool MediaList::RemoveAt(int iIndex, bool bDoNotDestroy) {
        DuiLib::CControlUI* pControl = GetItemAt(iIndex);
        if (nullptr == pControl) {
            return false;
        }

        if (0 != _tcsicmp(pControl->GetClass(), DUI_CTR_LISTCONTAINERELEMENT)) {
            return false;
        }

        auto pListElement = static_cast<DuiLib::CListContainerElementUI*>(pControl->GetInterface(DUI_CTR_LISTCONTAINERELEMENT));
        if (nullptr != pListElement) {
            auto pListItem = reinterpret_cast<ListItem*>(pListElement);
            if (nullptr != pListElement) {
                return Remove(pListElement, !bDoNotDestroy);
            }
        }
        else {
            return __super::RemoveAt(iIndex, bDoNotDestroy);
        }
        return false;
    }

    void MediaList::RemoveAll() {
        __super::RemoveAll();
        Clear();
    }

    bool MediaList::AddAt(CControlUI* pControl, int iIndex) {
        if (nullptr == pControl) {
            return false;
        }

        if (_tcsicmp(pControl->GetClass(), DUI_CTR_LISTCONTAINERELEMENT) != 0) {
            return false;
        }

        return __super::AddAt(pControl, iIndex);
    }

    bool MediaList::AddAt(ListItem* pListItem, int iIndex) {
        if (nullptr == pListItem || !pListItem->Init(this)) {
            return false;
        }

        DuiLib::CListContainerElementUI* pListElementUI = pListItem->GetListElement();
        if (nullptr == pListElementUI) {
            return false;
        }

        if (!__super::AddAt(pListElementUI, iIndex)) {
            return false;
        }

        return pListItem->SetMediaParent(m_pRoot);
    }

}