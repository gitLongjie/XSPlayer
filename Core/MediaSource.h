#pragma once

#include <memory>
#include <vector>
#include <memory>

#include "Core/Constant.h"
#include "Core/Media.h"

namespace XSPlayer {

    class MediaSourceCallback {
    public:
        virtual void OnLoadedCallback(Media* pMedia) = 0;
    };

    class MediaSource : public std::enable_shared_from_this<MediaSource>{
    public:
        virtual ~MediaSource(void) { }

        virtual bool Load(MediaSourceCallback* pCallback) = 0;
        virtual MediaContainer* GetMediaContainer(void) const = 0;
    };

    using MediaSourcePtr = std::shared_ptr<MediaSource>;
    using MediaSourceWPtr = std::weak_ptr<MediaSource>;

    class MediaSourceFactory {
    public:
        virtual MediaSourcePtr Create(void) = 0;
        virtual MediaSourceType GetSourceType(void) const = 0;
        virtual String GetSourceTypeString(void) const = 0;
    };


    

}

