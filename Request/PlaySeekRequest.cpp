#include "Request/PlaySeekRequest.h"

#include "Core/Application.h"
#include "Core/Log.h"
#include "IO/IOHandleChain.h"
#include "Contanst.h"

namespace XSPlayer {

    PlaySeekRequest::PlaySeekRequest(long long pos, bool bBroadcast) :supper(bBroadcast), m_seekPos(pos) {
    }


    PlaySeekRequest::~PlaySeekRequest(void) {

    }

    bool PlaySeekRequest::Vistor(MediaHandleChain* pHandleChain, bool& bNextChain) {
        __super::Vistor(pHandleChain, bNextChain);

        if (nullptr == pHandleChain) {
            return false;
        }

        return pHandleChain->HandleRequest(RequestID::RID_SEEK_PLAY, this);
    }

    long long PlaySeekRequest::GetSeekPosition(void) const {
        return m_seekPos;
    }

}
