#include "Render/RenderEvent.h"

#include "Core/Constant.h"
#include "Core/FFMpegContext.h"
#include "Render/AudioRenderChain.h"

namespace XSPlayer {


    RenderEvent::RenderEvent(Type type, float len) 
        : Event(EVENT_RENDER)
        , m_len(len)
        , m_type(type) {

    }

    RenderEvent::~RenderEvent(void) {

    }

    RenderEvent::Type RenderEvent::GetContent(void) const {
        return m_type;
    }

    float RenderEvent::GetLength(void) const {
        return m_len;
    }

    EventPtr RenderEvent::Create(RenderEvent::Type type, float len) {
        return std::make_shared<RenderEvent>(type, len);
    }

}
