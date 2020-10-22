#include "Core/ChainRequest.h"

namespace XSPlayer {

    ChainRequest::ChainRequest(bool bBroadcast) : m_bBroadcast(bBroadcast) {

    }

    ChainRequest::~ChainRequest() {

    }

    bool ChainRequest::Vistor(MediaHandleChain* pHandleChain, bool& bBroadcast) {
        bBroadcast = m_bBroadcast;
        return false;
    }

    bool ChainRequest::IsBroadcast(void) const {
        return m_bBroadcast;
    }

}
