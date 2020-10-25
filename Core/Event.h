#pragma once

#include <memory>
#include <string>

#include "Core/Constant.h"

namespace XSPlayer {

constexpr int EVENT_CONTROL = 1;
constexpr int EVENT_LRC = 2;
constexpr int EVENT_RENDER = 3;

    class Media;

    class Event : public std::enable_shared_from_this<Event> {
        NON_COPY_ABLE(Event)

    public:
        explicit Event(unsigned int id);
        virtual ~Event();

        unsigned int GetID(void) const { return m_id; }

    private:
        unsigned int m_id = 0;
    };
    using EventPtr = std::shared_ptr<Event>;

    class EventHandle {
    public:
        virtual ~EventHandle();
        virtual bool OnNotify(const EventPtr& pEvent) = 0;
    };

    class ControlEvent : public Event {
    public:
        enum class EControl {
            EC_UNKNOWN,
            EC_PLAY
        };
    public:
        virtual ~ControlEvent();
        EControl GetEC(void) const { return m_ec; }
        Media* GetMedia(void) const { return m_pMedia; }

        static EventPtr Create(Media* pMedia, EControl ec);

    public:
        explicit ControlEvent(Media* pMedia, EControl ec);

    private:
        Media* m_pMedia = nullptr;
        EControl m_ec = EControl::EC_UNKNOWN;
    };
}

