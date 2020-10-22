#pragma once

#include "Contanst.h"
#include "Core/Constant.h"

namespace XSPlayer {

    class BuildCallback : public DuiLib::IDialogBuilderCallback{
    public:
        BuildCallback(DuiLib::CPaintManagerUI* pPaintManagerUI);
        virtual ~BuildCallback();

        DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass) override;
        
    private:
        DuiLib::CPaintManagerUI* m_pPaintManagerUI;
    };
}
