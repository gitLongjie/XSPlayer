#include "Script/PyModule.h"

#include "Script/PyEnvironment.h"
#include "Script/PyAgrs.h"

#include <Python.h>

namespace XSPlayer {
    class PyModuleImpl {
    public:
        explicit PyModuleImpl(const char* szName) {
            m_pyImportModuleObject = PyImport_ImportModule(szName);
            if (nullptr == m_pyImportModuleObject) {
                PyEnvironment::TracebackError();
            }
        }

        ~PyModuleImpl() {
            if (nullptr != m_pyImportModuleObject) {
                PyEnvironment::PySalfRelease(m_pyImportModuleObject);
            }
        }

        PyObject* CallFunction(const char* szFunc, PyObject* pArgs) {
            if (nullptr == szFunc || nullptr == m_pyImportModuleObject) {
                return false;
            }

            PyObject* pAddFunc = PyObject_GetAttrString(m_pyImportModuleObject, szFunc);
            if (nullptr == pAddFunc) {
                return false;
            }

            PyObject* pObject = PyEval_CallObject(pAddFunc, pArgs);
            PyEnvironment::PySalfRelease(pAddFunc);
            return pObject;
        }

    private:
        PyObject* m_pyImportModuleObject = nullptr;
    };
    
    PyModule::PyModule(const char* szName) : m_pPyModuleImpl(new PyModuleImpl(szName)) {

    }

    PyModule::~PyModule(void) {
        if (nullptr != m_pPyModuleImpl) {
            delete m_pPyModuleImpl;
        }
    }

    bool PyModule::CallFunction(const char* method, const char* format, ...) {
        assert(nullptr != m_pPyModuleImpl);

        PyObject* pObject = nullptr;
        if (nullptr == format) {
            pObject = m_pPyModuleImpl->CallFunction(method, nullptr);
        }
        else {
            va_list vargs;
            va_start(vargs, format);
            PyObject* pArgs = Py_VaBuildValue(format, vargs);
            va_end(vargs);
            //p


            pObject = m_pPyModuleImpl->CallFunction(method,  pArgs);
        }
        if (nullptr == pObject) {
            PyEnvironment::TracebackError();
            return false;
        }
        PyEnvironment::PySalfRelease(pObject);
//         int nSize = PyTuple_Size(pReturn);
//         char* err_msg = nullptr;
//         PyArg_Parse(pReturn, "s", &err_msg);
//         PyEnvironment::PySalfRelease(pReturn);
//         PyEnvironment::PySalfRelease(pAddFunc);
//         PyEnvironment::PySalfRelease(pObject);
        return true;
    }

    bool PyModule::CallFunction(const char* method) {
        return CallFunction(method, nullptr);
    }

    bool PyModule::CallFunction(String& result, const char* method, const PyAgrs& args) {
        assert(nullptr != m_pPyModuleImpl);

        
        PyObject* pObject = m_pPyModuleImpl->CallFunction(method, reinterpret_cast<PyObject*>(args.GetContext()));
        if (nullptr == pObject) {
            PyEnvironment::TracebackError();
            return false;
        }

        char* szVal = nullptr;
        PyArg_Parse(pObject, "s", &szVal);
        result = nullptr == szVal ? "" : szVal;

        PyEnvironment::PySalfRelease(pObject);
        return true;
    }

    bool PyModule::CallFunction(String& result, const char* method) {
        return CallFunction(result, method, PyAgrs());
    }

}
