#pragma once

#include <memory>
#include "Core/Constant.h"
#include "Core/Singleton.h"


namespace XSPlayer {
    class PyModuleImpl;
    class PyAgrs;

    class PyModule {
    public:
        explicit PyModule(const char* szName);
        ~PyModule(void) ;
        
        bool CallFunction(const char* method, const char* format, ...);
        bool CallFunction(const char* method);
        bool CallFunction(String& result, const char* method, const PyAgrs& args);
        bool CallFunction(String& result, const char* method);

    private:
        PyModuleImpl* m_pPyModuleImpl;
    };
}
