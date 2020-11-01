#pragma once

#include "Contanst.h"
#include "Core/Event.h"

namespace XSPlayer {
    class UIEvent : public Event {
    public:
        explicit UIEvent(const DuiLib::TNotifyUI& msg);
        virtual ~UIEvent();

        const DuiLib::TNotifyUI& GetNotifyMessage(void) const;

        static EventPtr Create(const DuiLib::TNotifyUI& msg);

    private:
        const DuiLib::TNotifyUI& m_msg;

    };
}
