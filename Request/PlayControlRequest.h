#ifndef PLAYCONTROLREQUEST_H
#define PLAYCONTROLREQUEST_H

#include <memory>
#include "Core/Constant.h"
#include "Core/ChainRequest.h"

namespace XSPlayer {

    class PlayControlRequest : public ChainRequest {
        using supper = ChainRequest;
    public:
        enum class  Common {
            CtrlUnkown,
            CtrlPlay,
            CtrlPause,
            CtrlContinue,
            CtrlStop,
            CtrlEOF
        };
    public:
        explicit PlayControlRequest();
        ~PlayControlRequest(void) override;

        bool Vistor(MediaHandleChain* pHandleChain, bool& bNextChain) override;
        bool Play(const String& meidaFile);
        void Stop(void);
        bool PlayEOF(void);
        void Pause(void);
        void Continue(void);

        Common GetCommon(void) const { return m_ctrl; }
        const String& GetString(void) const { return m_mediaFile; }

    private:
        bool PlayInner(MediaHandleChain* pHandleChain, bool& bNextChain) const;
        bool PlayEOFInner(MediaHandleChain* pHandleChain, bool& bNextChain) const;
        void StopInner(MediaHandleChain* pHandleChain, bool& bNextChain) const;
        void PauseInner(MediaHandleChain* pHandleChain, bool& bNextChain) const;

        void ContinueInner(MediaHandleChain* pHandleChain, bool& bNextChain) const;
    private:
        String m_mediaFile;
        Common m_ctrl = Common::CtrlUnkown;
    };
    using PlayControlRequestPtr = std::shared_ptr<PlayControlRequest>;
}

#endif // PLAYCONTROLREQUEST_H
