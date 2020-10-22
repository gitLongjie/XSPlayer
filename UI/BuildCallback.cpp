#include "UI/BuildCallback.h"

#include <string>

#include "Core/MediaManager.h"
#include "UI/MediaList.h"

namespace XSPlayer {

    BuildCallback::BuildCallback(DuiLib::CPaintManagerUI* pPaintManagerUI)
        : m_pPaintManagerUI(pPaintManagerUI) {

    }

    BuildCallback::~BuildCallback() {

    }

    DuiLib::CControlUI* BuildCallback::CreateControl(LPCTSTR pstrClass) {
        if (0 == _tcsicmp(pstrClass, kMediaListUI)) {
            return new MediaList(m_pPaintManagerUI);
        }
        return nullptr;
    }

}