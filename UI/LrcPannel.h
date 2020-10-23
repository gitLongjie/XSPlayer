#pragma once

#include "Contanst.h"
#include "Core/Constant.h"

#include <chrono>

namespace XSPlayer {

    class LrcPannel : public DuiLib::CControlUI{
    public:
        LrcPannel();
        ~LrcPannel() override;

    public:
        bool DoPaint(HDC hDC, const RECT& rcPaint, DuiLib::CControlUI* pStopControl) override;

    };
}
