#include "MediaItem.h"

#include <assert.h>

#include "Core/Log.h"

namespace XSPlayer {

    MediaItem::MediaItem(Media* pParnet) : Media(pParnet) {

    }

    MediaItem::MediaItem(const String& text, Media* pParnet /*= nullptr*/)
        : Media(text, pParnet) {

    }

}
