#pragma once

#include "Contanst.h"
#include "Core/Constant.h"

#include <chrono>

namespace XSPlayer {

    class LrcItem {
    public:
        LrcItem(size_t tm, size_t index, const String content, const RECT& rect);
        void Draw(HDC hdc, int cur, const RECT& rect);
        int Compare(const float tm) const;
        void UpdateSize(size_t height);
    private:
        size_t m_tm;
        size_t m_index;
        String m_content;
        RECT m_rect;
    };

    class LrcPannel : public DuiLib::CControlUI {
    public:
        LrcPannel();
        ~LrcPannel() override;

    public:
        void PaintText(HDC hDC) override;
        void UpdateContent(const String& lrc);
        void UpdateLrc(float len);

    protected:
        bool DrawCurPlay(HDC hDC, const RECT& rect);
        bool DrawLrc(HDC hDC);

    private:
        void OnHandle(const String& lrc);
        int Analysis(const String& content, String& lrc) const;
        int GetCurrentLrc(float tm);
        void UpdateItemPostion(void);

    private:
        using LrcList = std::vector<LrcItem>;
        LrcList m_lrc;
        bool m_bUpdateSuccess = false;
        int m_lineHeight = 8;
        int m_curIndex = 0;

    };
}
