#pragma once

#include <duilib/UIlib.h>
#include <ShellAPI.h>
#include <string>

namespace XSPlayer {
    extern const TCHAR* const kClick;
    extern const TCHAR* const kSelectChanged;
    extern const TCHAR* const kCloseButtonControlName;
    extern const TCHAR* const kOfflineUI;
    extern const TCHAR* const kOnlineUI;
    extern const TCHAR* const kRightPannelUI;
    extern const TCHAR* const kMediaListUI;
    extern const TCHAR* const kListContainerElementUI;
    extern const TCHAR* const kbtnLoadMedia;
    extern const TCHAR* const kbtnLoadMediaFolder;
    extern const TCHAR* const kMenuLoadMedia;
    extern const TCHAR* const kMenuDeleteMedia;
    extern const TCHAR* const kMenuLoadMediaFolder;
    extern const TCHAR* const kbtnMediaPlay;

    extern const TCHAR* const kSliderValueChanged;

    extern const TCHAR* const kOfflineList;
    extern const TCHAR* const kOnlineList;

    extern const TCHAR* const kOfflineSource;

#define MAX_AUDIO_FRAME_SIZE 192000 

#define WM_OFFLINE_LOADMEIDA WM_USER + 50
#define WM_OFFLINE_ADD_LISTITEM (WM_OFFLINE_LOADMEIDA + 1)
#define WM_OFFLINE_NEXT (WM_OFFLINE_ADD_LISTITEM + 5)
#define WM_OFFLINE_PLAY (WM_OFFLINE_ADD_LISTITEM + 6)

#define WM_ONLINE_ADD_LISTITEM (WM_OFFLINE_LOADMEIDA + 20)
#define WM_ONLINE_ADD_MEDIAITEM (WM_ONLINE_ADD_LISTITEM + 1)
#define WM_ONLINE_NEXT (WM_ONLINE_ADD_LISTITEM + 5)
#define WM_ONLINE_PLAY (WM_ONLINE_ADD_LISTITEM + 6)

#define WM_RIGHTPANNEL_INIT_DURATION (WM_OFFLINE_LOADMEIDA + 30)
#define WM_RIGHTPANNEL_UPDATE_DURATION (WM_OFFLINE_LOADMEIDA + 31)

#define WM_CHANGE_CUR_PLAY (WM_OFFLINE_LOADMEIDA + 50)
#define WM_CHANGE_NEXT_PLAY (WM_OFFLINE_LOADMEIDA + 51)
#define WM_CHANGE_LAST_PLAY (WM_OFFLINE_LOADMEIDA + 52)
#define WM_CHANGE_STOP_PLAY (WM_OFFLINE_LOADMEIDA + 53)

#define IDT_TIMER1_1SECONDE 1

#define  WM_SHOWTASK WM_USER+501 
#define  WM_ONCLOSE WM_USER+502



#define HAND_WINDOW_MESSAGE(handle) \
if (nullptr != (handle)) { \
        hr = (handle)->HandleMessage(uMsg, wParam, lParam);\
    if (0 != hr) { \
        return hr; \
    } \
}

    enum class RequestID {
        RID_UNKOWN = 0,
        RID_FFMPEG_CONTEXT,
        RID_SEEK_PLAY,
        RID_CACHE_CONTROL,
        RID_PLAY_CONTROL,
    };

}
