#pragma once

#include "Contanst.h"
#include "Core/Media.h"

namespace XSPlayer {
    class ListItem;

    class MediaList : public DuiLib::CListUI {
    public:
        MediaList(DuiLib::CPaintManagerUI* pPaintManagerUI);
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

        bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true);

        Media* Find(const String& text) const;

        void Clear(void);
        DuiLib::CListContainerElementUI* CreateElementUI() const;

    protected:
        void UpdateStatus(int index);

    private:
        DuiLib::CPaintManagerUI* m_pPaintManagerUI;
        MediaContainer* m_pRoot = nullptr;
        static DuiLib::CDialogBuilder m_listElementBuilder;
    };

}
