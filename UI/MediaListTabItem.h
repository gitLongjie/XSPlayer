#pragma once

#include "Contanst.h"

namespace XSPlayer {

    class MediaListTabItem : public DuiLib::CContainerUI {
    public:
        MediaListTabItem();
        ~MediaListTabItem() override;

    private:
        void Init(void);

    };
}
