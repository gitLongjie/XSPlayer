#include "UI/MediaListTabItem.h"

#include "UI/BuildCallback.h"

#include <string>
#include <functional>
#include <algorithm>

#include<Shlobj.h>

#include "UI/Menu.h"
#include "UI/MediaList.h"
#include "UI/ListItem.h"
#include "UI/BuildCallback.h"

namespace XSPlayer {

    MediaListTabItem::MediaListTabItem() {
       // Init();
    }

    MediaListTabItem::~MediaListTabItem() {
        
    }

    void MediaListTabItem::Init(void) {
        DuiLib::CDialogBuilder dailogBuilder;
        BuildCallback callback(nullptr);
        DuiLib::CContainerUI* pContainerUI = dynamic_cast<DuiLib::CContainerUI*>(dailogBuilder.Create(_T("Data/skin/chinesestyle/media_list_tab_item.xml"), (UINT)0), &callback);
        if (nullptr != pContainerUI) {
            Add(pContainerUI);
        }
        else {
            RemoveAll();
        }
    }

}