#include "MediaFrame.h"

namespace XSPlayer {


    MediaFrame::MediaFrame(void) {

    }

    MediaFrame::~MediaFrame(void) {
        if (nullptr != m_pData) {
            delete[] m_pData;
        }
    }

    bool MediaFrame::Alloc(size_t len) {
        if (nullptr != m_pData || len != m_length) {
            delete[] m_pData;
            m_pData = nullptr;
        }

        m_length = len;
        m_pData = new unsigned char[len] {0};
        return true;
    }

    bool MediaFrame::Fill(const unsigned char* pData, const size_t len) {
        if (nullptr == pData || 0 == len) {
            return false;
        }

        if (nullptr != m_pData || len != m_length) {
            delete[] m_pData;
            m_pData = nullptr;
        }

        if (nullptr == m_pData) {
            m_length = len;
            m_pData = new unsigned char[len] {0};
        }
       
        m_length = len;
        memcpy(m_pData, pData, len);
        return true;
    }

    void MediaFrame::SetMediaFile(const String& meidaFile) {
        m_strMediaFile = meidaFile;
    }

}
