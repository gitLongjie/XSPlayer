#include "Request/PlayControlRequest.h"

#include "Core/Application.h"
#include "Core/Constant.h"
#include "Core/Log.h"
#include "IO/IOHandleChain.h"

namespace XSPlayer {

    PlayControlRequest::PlayControlRequest() :supper(true) {
    }


    PlayControlRequest::~PlayControlRequest(void) {

    }

    bool PlayControlRequest::Vistor(MediaHandleChain* pHandleChain, bool& bNextChain) {
        supper::Vistor(pHandleChain, bNextChain);
        if (nullptr == pHandleChain) {
            return false;
        }

        return pHandleChain->HandleRequest(RequestID::RID_PLAY_CONTROL, this);
        bool ret = false;
        switch (m_ctrl) {
        case  Common::CtrlUnkown:
            break;

        case  Common::CtrlPlay:
            ret = PlayInner(pHandleChain, bNextChain);
            break;

        case  Common::CtrlPause:
            PauseInner(pHandleChain, bNextChain);
            break;

        case  Common::CtrlContinue:
            ContinueInner(pHandleChain, bNextChain);
            break;

        case Common::CtrlEOF:

        case Common::CtrlStop:
            StopInner(pHandleChain, bNextChain);
            break;

        default:
            break;
        }

        return ret;
    }

    bool PlayControlRequest::Play(const String& meidaFile) {
        if (meidaFile == g_emptyString) {
            return false;
        }

        m_mediaFile = meidaFile;
        m_ctrl = Common::CtrlPlay;
        return true;
    }

    void PlayControlRequest::Stop(void) {
        m_ctrl = Common::CtrlStop;
    }

    bool PlayControlRequest::PlayEOF(void) {
        m_ctrl = Common::CtrlEOF;
        return true;
    }

    void PlayControlRequest::Pause(void) {
        m_ctrl = Common::CtrlPause;
    }

    void PlayControlRequest::Continue(void) {
        m_ctrl = Common::CtrlContinue;
    }

    bool PlayControlRequest::PlayInner(MediaHandleChain* pHandleChain, bool& bNextChain) const {
        IOHandleChain* pChaine = dynamic_cast<IOHandleChain*>(pHandleChain);
        if (nullptr == pChaine) {
            LOGE("play only run in IOHandleChain");
            return false;
        }

        bNextChain = false;
        return pChaine->OpenStream(m_mediaFile);
    }

    bool PlayControlRequest::PlayEOFInner(MediaHandleChain* pHandleChain, bool& bNextChain) const {
        if (nullptr == pHandleChain) {
            bNextChain = false;
            return false;
        }
        bNextChain = true;
        pHandleChain->HandleEOF();
        return true;
    }

    void PlayControlRequest::StopInner(MediaHandleChain* pHandleChain, bool& bNextChain) const {
        bNextChain = true;
        pHandleChain->Stop();
    }

    void PlayControlRequest::PauseInner(MediaHandleChain* pHandleChain, bool& bNextChain) const {
        bNextChain = true;
        pHandleChain->Pause();
    }

    void PlayControlRequest::ContinueInner(MediaHandleChain* pHandleChain, bool& bNextChain) const {
        bNextChain = true;
        pHandleChain->Continue();
    }

}
