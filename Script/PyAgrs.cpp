#include "Script/PyAgrs.h"

#include "Script/PyEnvironment.h"

#include <Python.h>

namespace XSPlayer {
    class PyAgrsImpl {
    public:
        explicit PyAgrsImpl(size_t size) : m_pTuple(PyTuple_New(size)){
        }

        ~PyAgrsImpl() {
            if (nullptr != m_pTuple) {
                PyEnvironment::PySalfRelease(m_pTuple);
            }
        }

        bool Add(const char* szArgs) {
            assert(nullptr != m_pTuple);
            return 0 == PyTuple_SetItem(m_pTuple, m_index++, Py_BuildValue("s", szArgs));
        }

        bool Add(int nArg) {
            assert(nullptr != m_pTuple);
            return 0 == PyTuple_SetItem(m_pTuple, m_index++, Py_BuildValue("i", nArg));
        }

        bool Set(size_t index, const char* szArg) {
            assert(nullptr != m_pTuple);
            return 0 == PyTuple_SetItem(m_pTuple, index, Py_BuildValue("i", szArg));
        }

        PyObject* GetTuple(void) {
            return m_pTuple;
        }

    private:
        PyObject* m_pTuple = nullptr;
        int m_index = 0;
    };
    
    PyAgrs::PyAgrs(size_t size) : m_pAgrsImpl(nullptr){
        if (size != 0) {
            m_pAgrsImpl = new PyAgrsImpl(size);
        }
    }

    PyAgrs::~PyAgrs(void) {
        if (nullptr != m_pAgrsImpl) {
            delete m_pAgrsImpl;
        }
    }

    void* PyAgrs::GetContext(void) const {
        if (nullptr == m_pAgrsImpl) {
            return nullptr;
        }

        return reinterpret_cast<void*>(m_pAgrsImpl->GetTuple());
    }

    bool PyAgrs::Add(const char* szArgs) {
        assert(nullptr != m_pAgrsImpl);
        return m_pAgrsImpl->Add(szArgs);
    }

    bool PyAgrs::Add(int nArgs) {
        assert(nullptr != m_pAgrsImpl);
        return m_pAgrsImpl->Add(nArgs);
    }

    bool PyAgrs::Set(size_t index, const char* szArgs) {
        assert(nullptr != m_pAgrsImpl);
        return m_pAgrsImpl->Set(index, szArgs);
    }

}
