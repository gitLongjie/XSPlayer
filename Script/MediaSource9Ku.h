#pragma once

#include "Core/Constant.h"
#include "Core/MediaSource.h"
#include "Core/MediaItem.h"


namespace XSPlayer {
    class PyModule;

    class MediaSource9Ku : public MediaSource {
        NON_COPY_ABLE(MediaSource9Ku)
    public:
        MediaSource9Ku();
        ~MediaSource9Ku();

    public:
        bool Load(MediaSourceCallback* pCallback) override;
        MediaContainer* GetMediaContainer(void) const override;

    public:
        bool BuilderMediaContents(MediaContainer* pMediaContainer, MediaSourceCallback* pCallback);
        bool BuilderMediaByType(MediaContainer* pMediaContainer, MediaSourceCallback* pCallback);
        void Test();

    protected:
        bool ParseMediaContents(const String& content, MediaContainer* pMediaContainer,
                                MediaSourceCallback* pCallback);
        bool ParseMediaItems(const String& content, MediaContainer* pMediaContainer,
                             MediaSourceCallback* pCallback);
        bool ParseMedia(const String& content, MediaContainer* pMediaContainer,
                             MediaSourceCallback* pCallback);

    private:
        void OnLoad(MediaSourceCallback* pCallback, MediaSourceWPtr pWMediaSource);
        void OnLoadMedia(const String url,
                         MediaContainer* pMediaContainer,
                         MediaSourceCallback* pCallback,
                         MediaSourceWPtr pWMediaSource);

    private:
        PyModule* m_pPyModule;
        std::thread::id m_threadId;
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
