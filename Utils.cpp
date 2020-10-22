#include "Utils.h"

#include <codecvt>

#include "Contanst.h"

namespace XSPlayer {
   
    bool Utils::GetFileNameByPath(const String& path, String& name) {
        size_t pot_pos = path.find_last_of('.');
        if (pot_pos == String::npos) {
            return false;
        }

        size_t dir_pos = path.find_last_of(_T("/\\"));
        if (String::npos == dir_pos) {
            return false;
        }
        dir_pos += 1;
        name = pot_pos > dir_pos ? path.substr(dir_pos, pot_pos - dir_pos) : _T("");
        return true;
    }

    std::wstring Utils::StringToWString(const std::string& str) {
        //setup converter
        typedef std::codecvt_utf8<wchar_t> convert_type;
        std::wstring_convert<convert_type, wchar_t> converter;

        //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
        return converter.from_bytes(str);
    }

    std::string Utils::WStringToString(const std::wstring& str) {
        std::string temp(str.length(), ' ');
        auto first = str.begin();
        auto last = str.end();
        auto result = temp.begin();
        while (first != last) {
            *result = static_cast<char>(*first);
            ++result; ++first;
        }
        
        return temp;
    }

    std::string Utils::UnicodeToGBK(const std::string& str, CP_TYPE type /*= CP_TYPE::CP_ACP*/) {
        std::wstring uncode = StringToWString(str);

        int len = WideCharToMultiByte(static_cast<UINT>(type), 0, uncode.c_str(), -1, nullptr, 0, nullptr, nullptr);
//        char* szTemp = new char[len + 1];
//        memset(szTemp, 0, len + 1);
        std::string res;
        res.resize(len + 1);
        WideCharToMultiByte(static_cast<UINT>(type), 0, uncode.c_str(), -1, const_cast<char*>(res.c_str()), len, nullptr, nullptr);
//        std::string strRet(szTemp);
//        delete[] szTemp;
        return res;
    }

    String Utils::GetAppPath() {
#if WIN32
        TCHAR szFileName[MAX_PATH];

        GetModuleFileName(NULL, szFileName, MAX_PATH);

        String path(szFileName);
        size_t dir_pos = path.find_last_of(_T("/\\"));
        if (String::npos == dir_pos) {
            return _T("");
        }
        return path.substr(0, dir_pos);
#endif
    }

}
