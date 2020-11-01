#pragma once

#include "Core/Constant.h"
#include "Core/MediaSource.h"
#include "Core/MediaItem.h"
#include "Core/Event.h"


namespace XSPlayer {
    class PyModule;
    class PyThreadContext;

    class MediaSource9Ku : public MediaSource, public EventHandle {
        NON_COPY_ABLE(MediaSource9Ku)
    public:
        MediaSource9Ku();
        ~MediaSource9Ku();

    public:
        bool Load(void) override;
        MediaContainer* GetMediaContainer(void) const override;
        bool OnNotify(const EventPtr& pEvent) override;

    public:
        bool BuilderMediaByType(MediaContainer* pMediaContainer,
                                PyModule* pPyModule);
        void Test();

    protected:
        bool ParseMediaContents(const String& content,
                                MediaContainer* pMediaContainer);
        bool ParseMediaItems(const String& content,
                             MediaContainer* pMediaContainer);
        bool ParseMedia(const String& content, MediaContainer* pMediaContainer);

    private:
        void OnLoad(MediaSourceWPtr pWMediaSource);
        void OnLoadMedia(const int songID,
                         MediaContainer* pMediaContainer,
                         MediaSourceWPtr pWMediaSource);
        void OnLoadLrcContent(const String mediaPath);

    private:
        std::thread::id m_threadId;
        std::thread::id m_loadSongThreadId;
        MediaContainer* m_pMediaContainer = nullptr;
        size_t m_count = 0;
    };

    class MediaSource9KuFactory : public MediaSourceFactory {
    public:
        MediaSourcePtr Create(void) override;
        MediaSourceType GetSourceType(void) const override;
        String GetSourceTypeString(void) const override;
    };
}
