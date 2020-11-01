#include "Core/Event.h"

#include <assert.h>

#include "Core/Log.h"

namespace XSPlayer {


    Event::Event(unsigned int id) : m_id(id){

    }

    Event::~Event() {

    }


    EventHandle::~EventHandle() {

    }


    ControlEvent::~ControlEvent() {

    }


    EventPtr ControlEvent::Create(Media* pMedia, EControl ec) {
        return std::make_shared<ControlEvent>(pMedia, ec);
    }

    ControlEvent::ControlEvent(Media* pMedia, EControl ec)
        : Event(1)
        , m_pMedia(pMedia)
        , m_ec(ec) {

    }


    const String& MediaSourceEvent::GetSource(void) const {
        return m_source;
    }

    EventPtr MediaSourceEvent::Create(Media* pMedia, const String& source) {
        return std::make_shared<MediaSourceEvent>(pMedia, source);
    }


    MediaSourceEvent::MediaSourceEvent(Media* pMedia, const String& source)
        : Event(EVENT_SOURCE)
        , m_pMedia(pMedia)
        , m_source(source) {

    }


    EventPtr MediaSourceTypeCreateEvent::Create(const String& txt, const String& source) {
        return std::make_shared<MediaSourceTypeCreateEvent>(txt, source);
    }

    MediaSourceTypeCreateEvent::MediaSourceTypeCreateEvent(const String& txt, const String& source)
        : Event(EVENT_SOURCE_TYPE)
        , m_txt(txt)
        , m_source(source) {

    }

}
