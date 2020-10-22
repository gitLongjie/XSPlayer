#pragma once

#include <memory>
#include <vector>

#include "Core/Constant.h"
#include "Core/Media.h"

namespace XSPlayer {

    class MediaItem : public Media {
        NON_COPY_ABLE(MediaItem)

    public:
        explicit MediaItem(Media* pParnet = nullptr);
        explicit MediaItem(const String& text, Media* pParnet = nullptr);

        void SetType(const String& type) { m_type = type; }
        const String& GetType(void) const { return m_type; }
        void SetArtist(const String& atrtist) { m_artist = atrtist; }
        const String& GetAirt(void) const { return m_artist; }

    private:
        uint m_uID;
        String m_artist;
        String m_type;
    };
    using MediaDatas = std::vector<MediaItem*>;

}

