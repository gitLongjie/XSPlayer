#pragma once

#include "Core/Event.h"

namespace XSPlayer {
    
    class RenderEvent : public Event {
    public:
        enum class Type {
            UNKNOWN,
            INIT,
            RENDER_POS
        };
    public:
        RenderEvent(Type type, float len);
        ~RenderEvent(void) override;

        Type GetContent(void) const;
        float GetLength(void) const;

        static EventPtr Create(Type type, float len);

    private:
        float m_len = 0;
        Type m_type = Type::UNKNOWN;
    };

}

