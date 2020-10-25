#pragma once

#include <memory>
#include "Core/Constant.h"
#include "Core/Event.h"


namespace XSPlayer {
    
    class LrcEvent : public Event {
    public:
        explicit LrcEvent(const String& lrc);
        ~LrcEvent(void) ;
        const String& GetContent(void) const { return m_strLrc; }

        static EventPtr Create(const String& lrc);
    private:
        String m_strLrc;
    };
}
