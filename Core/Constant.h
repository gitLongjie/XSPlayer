#ifndef CONSTANT_H
#define CONSTANT_H

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef unsigned int uint;

#include <string>
#include <thread>
#include <vector>

namespace XSPlayer {

#define UN_USE(param) (void)param;

#if defined(_UNICODE) || defined(UNICODE)
    typedef std::wstring String;
#else
    typedef std::string String;
#endif

    extern const String g_emptyString;

#define NON_COPY_ABLE(CLASSNAE) \
    CLASSNAE(const CLASSNAE&) = delete; \
    const CLASSNAE& operator=(const CLASSNAE&) = delete;

    enum class ThreadType {
        TT_UNKNOWN = 0,
        TT_IO,
        TT_DECODE,
        TT_MAIN, // only one thread
        TT_ENGINE // only one thread
    };

    extern std::thread::id INVALID_THREAD_ID;

    enum class MediaSourceType {
        MST_LOCAL,
        MST_9KU_SERVER
    };

     using MediaFile = std::pair<String, String>; // name, path
     using MediaFileList = std::vector<MediaFile>;
}

#endif // CONSTANT_H
