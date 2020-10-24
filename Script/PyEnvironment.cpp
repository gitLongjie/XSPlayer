#include "Script/PyEnvironment.h"

#include <Python.h>
#include <frameobject.h>

#include "Core/Application.h"
#include "Core/Log.h"
#include "IO/IOHandleChain.h"
#include "Utils.h"
#include "Script/MediaSource9Ku.h"

namespace XSPlayer {

    class PyThreadContext {
    public:
        PyThreadContext() {
            m_bHold = PyGILState_Check();   //检测当前线程是否拥有GIL
            if (!m_bHold) {
                m_gstate = PyGILState_Ensure();   //如果没有GIL，则申请获取GIL
            }

            m_save = PyEval_SaveThread();
            PyEval_RestoreThread(m_save);
        }

        ~PyThreadContext() {
            m_save = PyEval_SaveThread();
            PyEval_RestoreThread(m_save);
            if (!m_bHold) {
                PyGILState_Release(m_gstate);    //释放当前线程的GIL
            }
        }

    private:
        bool m_bHold = false;
        PyGILState_STATE m_gstate;

        PyThreadState* m_save = nullptr;
    };

    template<> PyEnvironment* Singleton<PyEnvironment>::m_pSingleton = nullptr;

    PyEnvironment::PyEnvironment() {
        Init();
    }


    PyEnvironment::~PyEnvironment(void) {

    }

    void PyEnvironment::AddSysPath(const String& path) {
        String sysPath = String("sys.path.append('") + path + String("')");
        RunSampleString("import sys");
#ifdef UNICODE
        PyRun_SimpleString(Utils::WStringToString(sysPath).c_str());
#else
        RunSampleString(sysPath.c_str());
#endif
    }

    int PyEnvironment::RunSampleString(const char* sampleString, int* flag /*= nullptr*/) {
        if (nullptr == flag) {
            return PyRun_SimpleString(sampleString);
        }
        else {
            PyCompilerFlags pyFlag = { *flag };
            return PyRun_SimpleStringFlags(sampleString, &pyFlag);
        }
    }

    void PyEnvironment::TracebackError(void) {
        PyObject* pyExcType;
        PyObject* pyExcValue;
        PyObject* pyExcTraceback;
        PyErr_Fetch(&pyExcType, &pyExcValue, &pyExcTraceback);
        PyErr_NormalizeException(&pyExcType, &pyExcValue, &pyExcTraceback);

        PyObject* str_exc_type = PyObject_Repr(pyExcType);
        PyObject* pyStr = PyUnicode_AsEncodedString(str_exc_type, "utf-8", "Error ~");
        const char* strExcType = PyBytes_AS_STRING(pyStr);

        PyObject* str_exc_value = PyObject_Repr(pyExcValue);
        PyObject* pyExcValueStr = PyUnicode_AsEncodedString(str_exc_value, "utf-8", "Error ~");
        const char* strExcValue = PyBytes_AS_STRING(pyExcValueStr);

        LOGE("python error:{} -> {}", strExcType, strExcValue);
#ifndef DEBUG      
        PySalfRelease(reinterpret_cast<void*>(pyExcType));
        PySalfRelease(reinterpret_cast<void*>(pyExcValue));
        PySalfRelease(reinterpret_cast<void*>(pyExcTraceback));

        PySalfRelease(reinterpret_cast<void*>(str_exc_type));
        PySalfRelease(reinterpret_cast<void*>(pyStr));

        PySalfRelease(reinterpret_cast<void*>(str_exc_value));
        PySalfRelease(reinterpret_cast<void*>(pyExcValueStr));
#endif
    }

    void PyEnvironment::PySalfRelease(void* pPyObject) {
        PyObject* pObject = reinterpret_cast<PyObject*>(pPyObject);
        if (nullptr == pObject) {
            return;
        }

#ifndef DEBUG
        Py_XDECREF(pObject);
#endif // !DEBUG
    }

    PyThreadContext* PyEnvironment::Create() {
        return new PyThreadContext();
    }

    void PyEnvironment::ReleaseThreadContext(PyThreadContext* pContext) {
        if (nullptr == pContext) {
            return;
        }

        delete pContext;
    }

    void PyEnvironment::Init(void) {
        // return;
        Py_Initialize();
        m_bInitSuccess = Py_IsInitialized() != 0;
        if (!m_bInitSuccess) {
            return;
        }

        PyEval_InitThreads();     //开启多线程支持
        int nInit = PyEval_ThreadsInitialized();  //检测线程支持是否开启成功
        if (nInit) {
            PyEval_SaveThread();  //因为调用PyEval_InitThreads成功后，当前线程就拥有了GIL，释放当前线程的GIL，
        }

// 
//         String strPath = Utils::GetAppPath() + "\\Python";
//         AddSysPath(strPath);
// 
//         MediaSource9Ku modul;
//         modul.Test();
 //       MediaContents meidaContents = std::move(module.BuilderMediaContents());

//         
//         try
//         {
//             
//             return;
//             PyRun_SimpleString("import sys");
// #ifdef UNICODE
//             PyRun_SimpleString(Utils::WStringToString(sysPath).c_str());
// #else
//             PyRun_SimpleString(sysPath.c_str());
// #endif
//             PyRun_SimpleString("print(sys.path)");
//             PyObject* pObject = PyImport_ImportModule("Python.9KuMusic");
//             if (nullptr == pObject) {
//                 TracebackError();
//                 return;
//             }
// 
//             PyObject* pAddFunc = PyObject_GetAttrString(pObject, "main");
//             if (nullptr == pAddFunc) {
//                 PySalfRelease(pObject);
//                 return;
//             }
// 
//              PyObject* pReturn = PyObject_CallFunction(pAddFunc,nullptr);
//              if (nullptr == pReturn) {
//                  PySalfRelease(pAddFunc);
//                  PySalfRelease(pObject);
//                  return;
//              }
//              int nSize = PyTuple_Size(pReturn);
//              char* err_msg = nullptr;
//              PyArg_Parse(pReturn, "s", &err_msg);
//              PySalfRelease(pReturn);
//              PySalfRelease(pAddFunc);
//              PySalfRelease(pObject);
//              return;
//         }
//         catch (...)
//         {
//             PyObject* ptype, * pvalue, * ptraceback;
// 
//             PyErr_Fetch(&ptype, &pvalue, &ptraceback);
// 
//             char err_msg[256] = {0};
//             PyArg_Parse(pvalue, "s", err_msg);
// 
//             printf(" python error:\n%s", err_msg);
//         }
    }

    void PyEnvironment::UnInit(void) {
        if (m_bInitSuccess) {
            Py_Finalize();
        }
    }

}
