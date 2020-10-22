#ifndef MEDIAFRAME_H
#define MEDIAFRAME_H

#include <memory>
#include <string>

#include "Core/Constant.h"

namespace XSPlayer {

    class MediaFrame {
        NON_COPY_ABLE(MediaFrame)

    public:
        MediaFrame(void);
        virtual ~MediaFrame(void);
        bool Alloc(size_t len);
        bool Fill(const unsigned char* pData, const size_t len);
        unsigned char* Data(void) const { return m_pData; }
        size_t GetLength(void) const { return m_length; }
        void SetMediaFile(const String& meidaFile);
        const String& GetMediaFile(void) const { return m_strMediaFile; }

        void SetOutSampleRate(int outSampleRate) { m_outSampleRate = outSampleRate; }
        int GetOutSampleRate(void) const { return m_outSampleRate; }
        void SetFormat(uint format) { m_format = format; }
        uint GetFormat(void) const { return m_format; }
        void SetChannel(int channel) { m_outChannel = channel; }
        int GetChannel(void) const { return m_outChannel; }
        void SetSample(int sample) { m_sample = sample; }
        int GetSample(void) const { return m_sample; }

    private:
        unsigned char* m_pData = nullptr;
        size_t m_length;
        String m_strMediaFile;
        int m_outSampleRate = 0;
        uint m_format = 0;
        int m_outChannel = 0;
        int m_sample = 0;
    };
    using MediaFramePtr = std::shared_ptr<MediaFrame>;
}

#endif // MEDIAFRAME_H
