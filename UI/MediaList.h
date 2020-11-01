#pragma once

#include "Contanst.h"
#include "Core/Media.h"
#include "Core/Event.h"

namespace XSPlayer {
    class ListItem;

    class MediaList : public DuiLib::CListUI {
    public:
        MediaList(const String& mediaSource);
        ~MediaList() override;

    public:
        bool Add(CControlUI* pControl);
        bool AddAt(CControlUI* pControl, int iIndex);
        bool Remove(CControlUI* pControl, bool bDoNotDestroy = false);
        bool RemoveAt(int iIndex, bool bDoNotDestroy);
        void RemoveAll();

        bool Add(ListItem* pListItem);
        bool AddAt(ListItem* pListItem, int iIndex);
        bool Remove(ListItem* pListItem, bool bDoNotDestroy = false);
        bool Add(const String& mediaSource, Media* pMedia);

        bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true);

        Media* Find(const String& text) const;

        void Clear(void);
        DuiLib::CListContainerElementUI* CreateElementUI() const;
        bool Compare(const String& mediaSource) const;
        void UnInit(void);

    protected:
        void UpdateStatus(int index);

    private:
        MediaContainer* m_pRoot = nullptr;
        const String m_strMediaSource;
        static DuiLib::CDialogBuilder m_listElementBuilder;
    };

}
