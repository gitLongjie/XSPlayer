#include "UI/UIEvent.h"

namespace XSPlayer {


    UIEvent::UIEvent(const DuiLib::TNotifyUI& msg) :Event(EVENT_UI), m_msg(msg) {

    }

    UIEvent::~UIEvent() {

    }

    const DuiLib::TNotifyUI& UIEvent::GetNotifyMessage(void) const {
        return m_msg;
    }

    EventPtr UIEvent::Create(const DuiLib::TNotifyUI& msg) {
        return std::make_shared<UIEvent>(msg);
    }

}