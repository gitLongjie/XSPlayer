#ifndef MEDIAHANDLETASK_H
#define MEDIAHANDLETASK_H

#include <memory>
#include <string>

#include "Core/Constant.h"
#include "Core/Task.h"
#include "Core/MediaHandleChain.h"

namespace XSPlayer {

    class MediaHandleTask : public Task{
        NON_COPY_ABLE(MediaHandleTask)

    public:
        MediaHandleTask();
        ~MediaHandleTask(void) override ;

//        bool HandleMediaFrame(const MediaFramePtr& pMediaFrame);
        void OnRun(void) override;
    //    MediaFramePtr GetMediaFrame(void) const { return m_pMediaFrame; }


    protected:
        virtual void DoTask(void) = 0;

    protected:
  //      MediaFramePtr m_pMediaFrame;

    private:
    };

    using MediaHandleTaskPtr = std::shared_ptr<MediaHandleTask>;
}

#endif // MEDIAHANDLETASK_H
