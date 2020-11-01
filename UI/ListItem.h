#pragma once

#include "Contanst.h"
#include "Core/Media.h"

namespace XSPlayer {
    class MediaList;

    class ListItem {
    public:
        ListItem(MediaList* pMediaList = nullptr);
        virtual ~ListItem();

        DuiLib::CListContainerElementUI* GetListElement(void) const;
        void SetMediaList(MediaList* pMediaList);
        MediaList* GetMediaList(void) const;

        virtual bool Init(MediaList* pMediaList);
        virtual void Expand(bool bExpend);
        virtual void OnExpend(bool bExpend);
        virtual bool SetMediaParent(Media* pMedia) = 0;
        virtual bool HasChildren(void) const;

        bool IsInited(void) const { return m_bInited; }
        bool IsExpand(void) const { return m_bExpand; }

        bool Notify(void* pArgs);


    protected:
        virtual DuiLib::CListContainerElementUI* CreateUI(void) = 0;
        bool OnNotify(const DuiLib::TNotifyUI& event);
        virtual bool OnItemClick(DuiLib::CControlUI* pControl);

    private:
        DuiLib::CListContainerElementUI* m_pListElementUI = nullptr;
        MediaList* m_pMediaList = nullptr;
        bool m_bInited = false;
        bool m_bExpand = true;
        std::chrono::steady_clock::time_point m_lastEventTimePoint;
    };

    class MediaListItemContainer : public MediaContainer, public ListItem{
    public:
        MediaListItemContainer(const String& strText, Media* pParnet = nullptr);
        ~MediaListItemContainer() override;
        
    public:
        bool Init(MediaList* pMediaList) override;
        bool Add(Media* pMedia) override;
        void Expand(bool bExpend) override;
        bool SetMediaParent(Media* pMedia) override;
        bool HasChildren(void) const override;
        void SetText(const String& text);
        void SetMediaID(unsigned int mediaID);

    protected:
        DuiLib::CListContainerElementUI* CreateUI(void) override;
        void OnExpend(bool bExpend) override;

    protected:
        bool InsertToMediaList(Media* pMedia);

    private:
        DuiLib::CLabelUI* m_pExpendLable = nullptr;
        DuiLib::CLabelUI* m_pTextLable = nullptr;
        void UpdateExpandIcon();
    };

    class MediaListItem : public Media, public ListItem {
    public:
        MediaListItem(const String& strText, Media* pParnet = nullptr);
        ~MediaListItem() override;

    public:
        bool SetMediaParent(Media* pMedia) override;
        void SetImage(const String& strImagePath);
        const String& GetImage(void) const;
        void SetText(const String& text);        
        void SetMediaArtist(const String& strMediaArtist);
        const String& GetMediaArtist(void) const;
        void SetMediaPath(const String& meidaPath);
        const String& GetMediaPath(void) const;
        void SetStore(bool bStore);
        bool IsStore(void) const;
        void SetPlaying(bool bPlay);

        void SetMediaID(unsigned int mediaID);

    protected:
        DuiLib::CListContainerElementUI* CreateUI(void) override;
        bool OnItemClick(DuiLib::CControlUI* pControl) override;

    private:
        String m_strImage;
        String m_strArtist = _T("δ֪");
        String m_strMediaPath;
        bool m_bStore = false;
        bool m_bPlaying = false;
        DuiLib::CLabelUI* m_pMediaImageLabel = nullptr;
        DuiLib::CLabelUI* m_pMediaNameLabel = nullptr;
        DuiLib::CLabelUI* m_pArtistNameLabel = nullptr;
        DuiLib::CButtonUI* m_pStoreButton = nullptr;
        DuiLib::CLabelUI* m_pPlayStatusLabel = nullptr;
    };
}
