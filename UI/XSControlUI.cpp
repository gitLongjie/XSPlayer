#include "UI/XSControlUI.h"

namespace XSPlayer {

    XSControlUI::XSControlUI(void) {

    }

    XSControlUI::~XSControlUI(void) {
        UnInitWindow();
    }

    void XSControlUI::InitWindow(void) {
        if (nullptr == m_pManager) {
            return;
        }

        m_pManager->AddNotifier(this);
    }

    void XSControlUI::UnInitWindow(void) {
        if (nullptr == m_pManager) {
            return;
        }

        m_pManager->RemoveNotifier(this);
    }

    LRESULT XSControlUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        return 0;
    }

}
