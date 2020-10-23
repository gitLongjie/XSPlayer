#include "UI/LrcPannel.h"

#include <string>

#include "Core/MediaManager.h"

namespace XSPlayer {

    LrcPannel::LrcPannel() {

    }

    LrcPannel::~LrcPannel() {

    }

    bool LrcPannel::DoPaint(HDC hDC, const RECT& rcPaint, DuiLib::CControlUI* pStopControl) {
        RECT rect(rcPaint);
        FillRect(hDC, &rect, HBRUSH(COLOR_WINDOW + 1));
        
        TCHAR sz[] = _T("hrekadj\r\nkadjkfja\r\njadfaj\r\njadfaj\r\n");
        DrawText(hDC, sz, sizeof(sz), &rect, DT_CENTER);

        
        return __super::DoPaint(hDC, rcPaint, pStopControl);
    }

}