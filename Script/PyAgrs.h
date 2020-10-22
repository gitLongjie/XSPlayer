#pragma once

#include <memory>
#include "Core/Constant.h"


namespace XSPlayer {
    class PyAgrsImpl;

    class PyAgrs {
    public:
        explicit PyAgrs(size_t size = 0);
        ~PyAgrs(void) ;
        void* GetContext(void) const;
        bool Add(const char* szArgs);
        bool Add(int nArgs);

        bool Set(size_t index, const char* szArgs);

    private:
        PyAgrsImpl* m_pAgrsImpl;
    };
}
