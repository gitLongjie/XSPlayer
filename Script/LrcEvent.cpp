#include "Script/LrcEvent.h"


namespace XSPlayer {
    
    LrcEvent::LrcEvent(const String& lrc) : Event(EVENT_LRC), m_strLrc(lrc) {

    }

    LrcEvent::~LrcEvent(void) {

    }

    EventPtr LrcEvent::Create(const String& lrc) {
        return std::make_shared<LrcEvent>(lrc);
    }

}
