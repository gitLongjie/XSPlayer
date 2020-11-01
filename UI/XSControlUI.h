#pragma once

#include "Contanst.h"

namespace XSPlayer {

    class XSControlUI : public DuiLib::CContainerUI, public DuiLib::INotifyUI {
    public:
        explicit XSControlUI(void);

        ~XSControlUI(void) override;
        virtual void InitWindow(void);
        virtual void UnInitWindow(void);
        virtual  LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    };
}
