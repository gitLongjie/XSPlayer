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
        RenderEvent(Type type, size_t len);
        ~RenderEvent(void) override;

        Type GetContent(void) const;
        size_t GetLength(void) const;

        static EventPtr Create(Type type, size_t len);

    private:
        size_t m_len = 0;
        Type m_type = Type::UNKNOWN;
    };

}

