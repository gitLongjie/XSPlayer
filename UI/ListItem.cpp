#include "UI/ListItem.h"

#include "Core/MediaManager.h"

#include "MediaList.h"
#include "UI/UIEvent.h"

namespace XSPlayer {
    ListItem::ListItem(MediaList* pMediaList /*= nullptr*/) 
        : m_pMediaList(pMediaList)
        , m_lastEventTimePoint(std::chrono::high_resolution_clock::now()) {

    }

    ListItem::~ListItem() {

    }

    DuiLib::CListContainerElementUI* ListItem::GetListElement(void) const {
        return m_pListElementUI;
    }

    void ListItem::SetMediaList(MediaList* pMediaList) {
        m_pMediaList = pMediaList;
    }

    MediaList* ListItem::GetMediaList(void) const {
        return m_pMediaList;
    }

    bool ListItem::Init(MediaList* pMediaList) {
        if (m_bInited) {
            return true;
        }

        if (nullptr != m_pMediaList && m_pMediaList != pMediaList) {
            m_pMediaList->Remove(this, true);
        }
        m_pMediaList = pMediaList;
        m_bInited = true;

        m_pListElementUI = CreateUI();
        if (nullptr == m_pListElementUI) {
            return false;
        }
        
        m_pListElementUI->SetTag(reinterpret_cast<UINT_PTR>(this));
        m_pListElementUI->OnNotify += DuiLib::MakeDelegate<ListItem, ListItem>(this, &ListItem::Notify);
        return m_bInited;
    }

    void ListItem::Expand(bool bExpend) {
        m_bExpand = bExpend;
    }

    void ListItem::OnExpend(bool bExpend) {
        if (nullptr == m_pListElementUI) {
            return;
        }

        m_pListElementUI->SetVisible(bExpend);
    }

    bool ListItem::HasChildren(void) const {
        return false;
    }

    bool ListItem::Notify(void* pArgs) {
        DuiLib::TNotifyUI* pNotifyUI = static_cast<DuiLib::TNotifyUI*>(pArgs);
        if (nullptr == pNotifyUI) {
            return false;
        }

        return OnNotify(*pNotifyUI);
    }

    bool ListItem::OnNotify(const DuiLib::TNotifyUI& event) {
        if (0 == _tcsicmp(event.sType, _T("itemclick"))) {
            return OnItemClick(event.pSender);
        }
        else if (0 == _tcsicmp(event.sType, _T("click"))) {
            return OnItemClick(event.pSender);
        }
        else if (0 == _tcsicmp(event.sType, _T("itemactivate"))) {
            
        }

        MediaManager::GetSingleton().NotifyEvent(UIEvent::Create(event));
        return false;
    }

    bool ListItem::OnItemClick(DuiLib::CControlUI* pControl) {
        if (m_pListElementUI != pControl) {
            return false;
        }

        auto time_point = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_point - m_lastEventTimePoint).count();
        if (duration < 300) {
            return false;
        }
        m_lastEventTimePoint = time_point;

        if (m_bExpand) {
            Expand(false);
        }
        else {
            Expand(true);
        }
        
        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    MediaListItemContainer::MediaListItemContainer(const String& text, Media* pParnet /*= nullptr*/)
        : MediaContainer(pParnet) {
        SetText(text);
    }

    MediaListItemContainer::~MediaListItemContainer() {

    }

    bool MediaListItemContainer::Init(MediaList* pMediaList) {
        if (!__super::Init(pMediaList)) {
            return false;
        }

        if (GetChildernCount() > 0) {
            UpdateExpandIcon();
        }
        return true;
    }

    bool MediaListItemContainer::Add(Media* pMedia) {
        if (nullptr == pMedia) {
            return false;
        }

        if (__super::Add(pMedia)) {
            InsertToMediaList(pMedia);
            UpdateExpandIcon();
        }

        return true;
    }

    void MediaListItemContainer::SetText(const String& text) {
        __super::SetText(text);

        if (nullptr != m_pTextLable) {
            m_pTextLable->SetText(text.c_str());
        }
    }

    void MediaListItemContainer::SetMediaID(unsigned int mediaID) {
        __super::SetMediaID(mediaID);
        DuiLib::CListContainerElementUI* pElementUI = GetListElement();
        if (nullptr == pElementUI) {
            return;
        }
#if defined(UNICODE) || defined(_UNICODE)
        pElementUI->SetUserData(std::to_wstring(mediaID).c_str());
#else
        pElementUI->SetUserData(std::to_string(mediaID).c_str());
#endif
    }

    void MediaListItemContainer::Expand(bool bExpend) {
        size_t count = GetChildernCount();
        for (size_t index = 0; index < count; ++index) {
            Media* pMedia = GetChild(index);
            auto pListItem = dynamic_cast<ListItem*>(pMedia);
            if (nullptr == pListItem ) {
                continue;
            }

            pListItem->OnExpend(bExpend);
        }

        __super::Expand(bExpend);

        UpdateExpandIcon();
    }

    bool MediaListItemContainer::SetMediaParent(Media* pMedia) {
        if (nullptr != GetParent()) {
            return true;
        }

        auto pMediaContainer = dynamic_cast<MediaContainer*>(pMedia);
        if (nullptr == pMediaContainer) {
            return false;
        }

        return pMediaContainer->Add(this);
    }

    bool MediaListItemContainer::HasChildren(void) const {
        return true;
    }

    DuiLib::CListContainerElementUI* MediaListItemContainer::CreateUI(void) {
        auto pHorizontalLayout = new DuiLib::CHorizontalLayoutUI;
        pHorizontalLayout->SetFixedHeight(22);

        auto pVerticalLayout = new DuiLib::CVerticalLayoutUI;
        pHorizontalLayout->Add(pVerticalLayout);
        pVerticalLayout->SetFixedWidth(15);
        auto pControl = new DuiLib::CControlUI;
        pVerticalLayout->Add(pControl);
        pControl->SetFixedHeight(7);
        m_pExpendLable = new DuiLib::CLabelUI;
        pVerticalLayout->Add(m_pExpendLable);
        m_pExpendLable->SetMaxWidth(9);
        m_pExpendLable->SetMaxHeight(9);

        pVerticalLayout = new DuiLib::CVerticalLayoutUI;
        pHorizontalLayout->Add(pVerticalLayout);
        m_pTextLable = new DuiLib::CLabelUI;
        pVerticalLayout->Add(m_pTextLable);
        m_pTextLable->SetTextColor(0xFFFFFFFF);
        m_pTextLable->SetText(GetText().c_str());

        auto pListElementUI = new DuiLib::CListContainerElementUI;
        pListElementUI->Add(pHorizontalLayout);
        pListElementUI->SetFixedHeight(22);

        size_t mediaID = GetMediaId();
#if defined(UNICODE) || defined(_UNICODE)
        pListElementUI->SetUserData(std::to_wstring(mediaID).c_str());
#else
        pListElementUI->SetUserData(std::to_string(mediaID).c_str());
#endif
        return pListElementUI;
    }

    void MediaListItemContainer::OnExpend(bool bExpend) {
        size_t count = GetChildernCount();
        for (size_t index = 0; index < count; ++index) {
            Media* pMedia = GetChild(index);
            auto pListItem = dynamic_cast<ListItem*>(pMedia);
            if (nullptr == pListItem || !pMedia->IsLeaf()) {
                continue;
            }

            pListItem->OnExpend(bExpend);
        }

        __super::OnExpend(bExpend);
    }

    bool MediaListItemContainer::InsertToMediaList(Media* pMedia) {
        MediaList* pMediaList = GetMediaList();
        if (nullptr == pMediaList) {
            return false;
        }

        auto pListItem = dynamic_cast<ListItem*>(pMedia);
        if (nullptr == pListItem) {
            return false;
        }

        DuiLib::CListContainerElementUI* pListElement = GetListElement();
        if (nullptr == pListElement) {
            return false;
        }

        int index = pMediaList->GetItemIndex(pListElement);
        size_t count = GetChildernCount();

        int insertPos = index + static_cast<int>(count);
        return pMediaList->AddAt(pListItem, insertPos);
    }

    void MediaListItemContainer::UpdateExpandIcon() {
        if (nullptr == m_pExpendLable) {
            return;
        }

        if (IsExpand()) {
            m_pExpendLable->SetBkImage(_T("Data/skin/image/list_icon_b.png"));
        }
        else {
            m_pExpendLable->SetBkImage(_T("Data/skin/image/list_icon_a.png"));
        }
    }

    //////////////////////////////////////////////////////////////////////////
    MediaListItem::MediaListItem(const String& strText, Media* pParnet /*= nullptr*/)
        : Media(strText, pParnet) {

    }

    MediaListItem::~MediaListItem() {

    }

    bool MediaListItem::SetMediaParent(Media* pMedia) {
        if (nullptr != GetParent()) {
            return true;
        }

        auto pMediaContainer = dynamic_cast<MediaContainer*>(pMedia);
        if (nullptr == pMediaContainer) {
            return false;
        }

        return pMediaContainer->Add(this);
    }

    void MediaListItem::SetImage(const String& strImagePath) {
        m_strImage = strImagePath;
        if (nullptr == m_pMediaImageLabel) {
            return;
        }

        m_pMediaImageLabel->SetBkImage(strImagePath.c_str());
    }

    const String& MediaListItem::GetImage(void) const {
        return m_strImage;
    }

    void MediaListItem::SetText(const String& text) {
        __super::SetText(text);
        if (nullptr == m_pMediaNameLabel) {
            return;
        }

        m_pMediaNameLabel->SetText(text.c_str());
    }

    void MediaListItem::SetMediaArtist(const String& strMediaArtist) {
        m_strArtist = strMediaArtist;
        if (nullptr == m_pArtistNameLabel) {
            return;
        }

        m_pArtistNameLabel->SetText(strMediaArtist.c_str());
    }

    const String& MediaListItem::GetMediaArtist(void) const {
        return m_strArtist;
    }

    void MediaListItem::SetMediaPath(const String& meidaPath) {
        m_strMediaPath = meidaPath;
    }

    const String& MediaListItem::GetMediaPath(void) const {
        return m_strMediaPath;
    }

    void MediaListItem::SetStore(bool bStore) {
        m_bStore = bStore;
        if (nullptr == m_pStoreButton) {
            return;
        }

        if (m_bStore) {
            m_pStoreButton->SetNormalImage(_T("Data/skin/image/love_stored.png"));
            m_pStoreButton->SetHotImage(_T("Data/skin/image/love_store.png"));
            m_pStoreButton->SetPushedImage(_T("Data/skin/image/love_store.png"));
        }
        else {
            m_pStoreButton->SetNormalImage(_T("Data/skin/image/love_store.png"));
            m_pStoreButton->SetHotImage(_T("Data/skin/image/love_stored.png"));
            m_pStoreButton->SetPushedImage(_T("Data/skin/image/love_stored.png"));
        }
        
    }

    bool MediaListItem::IsStore(void) const {
        return m_bStore;
    }

    void MediaListItem::SetPlaying(bool bPlay) {
        m_bPlaying = bPlay;
        if (nullptr == m_pPlayStatusLabel) {
            return;
        }

        String text(_T("正在播放"));
        if (!bPlay) {
            text = _T("");
        }
        m_pPlayStatusLabel->SetText(text.c_str());
    }

    void MediaListItem::SetMediaID(unsigned int mediaID) {
        __super::SetMediaID(mediaID);

        DuiLib::CListContainerElementUI* pElementUI = GetListElement();
        if (nullptr == pElementUI) {
            return;
        }
#if defined(UNICODE) || defined(_UNICODE)
        pElementUI->SetUserData(std::to_wstring(mediaID).c_str());
#else
        pElementUI->SetUserData(std::to_string(mediaID).c_str());
#endif
    }

    DuiLib::CListContainerElementUI* MediaListItem::CreateUI(void) {
        MediaList* pMediaList= GetMediaList();
        if (nullptr == pMediaList) {
            return nullptr;
        }

        DuiLib::CListContainerElementUI* pElementUI = pMediaList->CreateElementUI();
        if (nullptr == pElementUI) {
            return nullptr;
        }

        size_t mediaId = GetMediaId();
#if defined(UNICODE) || defined(_UNICODE)
        pElementUI->SetUserData(std::to_wstring(mediaId).c_str());
#else
        pElementUI->SetUserData(std::to_string(mediaId).c_str());
#endif
        
        pElementUI->SetFixedHeight(40);

        m_pMediaImageLabel = dynamic_cast<DuiLib::CLabelUI*>(pElementUI->FindSubControl(_T("media_image")));
        m_pMediaNameLabel = dynamic_cast<DuiLib::CLabelUI*>(pElementUI->FindSubControl(_T("media_name")));
        m_pArtistNameLabel = dynamic_cast<DuiLib::CLabelUI*>(pElementUI->FindSubControl(_T("artist_name")));
        m_pStoreButton = dynamic_cast<DuiLib::CButtonUI*>(pElementUI->FindSubControl(_T("media_store")));
        m_pPlayStatusLabel = dynamic_cast<DuiLib::CLabelUI*>(pElementUI->FindSubControl(_T("play_status")));

        if (m_strImage.empty()) {
            SetImage(_T("Data/skin/image/head.jpg"));
        }
        else {
            SetImage(m_strImage);
        }
        m_pMediaNameLabel->SetText(GetText().c_str());
        SetMediaArtist(m_strArtist);
        SetStore(m_bStore);
        SetPlaying(m_bPlaying);

        if (nullptr != m_pStoreButton) {
            m_pStoreButton->OnNotify += DuiLib::MakeDelegate<ListItem, ListItem>(this, &ListItem::Notify);
        }

        return pElementUI;
    }

    bool MediaListItem::OnItemClick(DuiLib::CControlUI* pControl) {
        if (__super::OnItemClick(pControl)) {
            return true;
        }

        if (m_pStoreButton == pControl) {
            SetStore(!m_bStore);
            return true;
        }

        return false;
    }

}