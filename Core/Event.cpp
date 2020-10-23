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
    , m_ec(ec){

    }

}
