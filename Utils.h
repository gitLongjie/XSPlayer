#pragma once

#include "Core/Constant.h"

namespace XSPlayer {
    class Utils {
    public:
        enum class CP_TYPE {
            ACP=0,           // default to ANSI code page
            OEMCP,           // default to OEM  code page
            MACCP,          // default to MAC  code page
            THREAD,         // current thread's ANSI code page_ACP,      
            SYMBOL = 42,    // SYMBOL translations
            UTF7 = 65000,   // UTF-7 translation 
            UTF8 = 65001,   // UTF-8 translation
        };
    public:
        static bool GetFileNameByPath(const String& path, String& name);
        static std::wstring StringToWString(const std::string& str);
        static std::string WStringToString(const std::wstring& str);
        static std::string UnicodeToGBK(const std::string& str, CP_TYPE type = CP_TYPE::ACP);
        static String GetAppPath();
    };
}
