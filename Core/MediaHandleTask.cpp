#include "Core/MediaHandleTask.h"

namespace XSPlayer {

    MediaHandleTask::MediaHandleTask(/*const MediaHandleChainPtr& pHandleChain*/) {
   // : m_pHandleChain(pHandleChain) {

    }

    MediaHandleTask::~MediaHandleTask(void) {

    }
// 
//     bool MediaHandleTask::HandleMediaFrame(const MediaFramePtr& pMediaFrame) {
//         MediaHandleChainPtr pMediaChain = m_pHandleChain.lock();
//         if (nullptr != pMediaChain) {
//             pMediaChain->HandleMediaFrame(pMediaFrame);
//             return true;
//         }
//         return false;
//     }

    void MediaHandleTask::OnRun(void) {
        DoTask();
// 
//         MediaHandleChainPtr pMediaChain = m_pHandleChain.lock();
//         if (nullptr != pMediaChain) {
//             pMediaChain->OnNextChain(this);
//         }
    }

}
