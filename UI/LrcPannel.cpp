#include "UI/LrcPannel.h"

#include <string>
#include <regex>

#include "Core/MediaManager.h"

namespace XSPlayer {

    LrcPannel::LrcPannel() {
//         String lrc;
//         Analysis("[00:15.21] 是谁让我孕育了爱情", lrc);
    }

    LrcPannel::~LrcPannel() {

    }

    void LrcPannel::PaintText(HDC hDC) {
        RECT rect(m_rcItem);
        if (!m_bUpdateSuccess) {
            return;
        }
        
        ::SetBkMode(hDC, TRANSPARENT);
        DrawLrc(hDC);
    }

    void LrcPannel::UpdateContent(const String& lrc) {
        m_lrc.clear();
        OnHandle(lrc);
    }

    void LrcPannel::UpdateLrc(float tm) {
        int index = GetCurrentLrc(tm);
        if (m_curIndex == index) {
            return;
        }

        m_curIndex = index;
        UpdateItemPostion();
        Invalidate();
    }

    bool LrcPannel::DrawCurPlay(HDC hDC, const RECT& rect) {
        COLORREF old = SetTextColor(hDC, RGB(143, 250, 200));
        int height = (rect.bottom - rect.top) >> 1;
        //int height = (rect.bottom - rect.top);
//         for (int index = m_index; index < m_lrc.size(); ++index) {
//             RECT drawRect = { rect.left, height - m_lineHeight, rect.right, height + m_lineHeight };
//             // FillRect(hDC, &drawRect, HBRUSH(COLOR_WINDOW + 1));
// 
//  //           DrawText(hDC, m_lrc[index].second.c_str(), -1, &drawRect, DT_CENTER);
//             height += m_lineHeight * 2;
//         }
//         
//         SetTextColor(hDC, old);
        return true;
    }

    bool LrcPannel::DrawLrc(HDC hDC) {
        COLORREF old = SetTextColor(hDC, RGB(43, 94, 140));
        for (auto& item : m_lrc) {
            item.Draw(hDC, m_curIndex - 1 , m_rcItem);
        }
        SetTextColor(hDC, old);

        return true;
    }

    void LrcPannel::OnHandle(const String& strLrc) {
        std::regex regex("\n");
        std::vector<String> v(std::sregex_token_iterator(strLrc.begin(), strLrc.end(), regex, -1), std::sregex_token_iterator());
        if (v.empty()) {
            m_bUpdateSuccess = false;
        }
        else {
            m_bUpdateSuccess = true;
        }

        int nHalfHeight = (m_rcItem.bottom - m_rcItem.top) >> 1;

        size_t index = 0;
        for (const auto & item : v) {
            String content;
            int tm = Analysis(item, content);
            if (-1 != tm) {
                m_lrc.emplace_back(std::move(LrcItem(tm, index++, content,
                                   { m_rcItem.left, nHalfHeight - m_lineHeight,
                                   m_rcItem.right, nHalfHeight + m_lineHeight })));

                nHalfHeight += m_lineHeight << 1;
            }
        }
    }

    int LrcPannel::Analysis(const String& content, String& lrc) const {
        size_t bpos = content.find("[") + 1;
        size_t epos = content.find("]");

        if (epos == bpos) {
            return -1;
        }

        String time = content.substr(bpos, epos - 1);

        lrc = content.substr(epos + 1);

        int mutie = 0;
        int scend = 0;
        int mscend = 0;
        if (0 == sscanf(time.c_str(), "%d:%d.%d", &mutie, &scend, &mscend)) {
            return -1;
        }

        return mutie * 60 + scend;
    }

    int LrcPannel::GetCurrentLrc(float tm) {
        if (0 == tm) {
            return 0;
        }

        int index = 0;
        for (const auto& item : m_lrc) {
            if ( -1 == item.Compare(tm)) {
                return index;
            }

            ++index;
        }
        return index;
    }

    void LrcPannel::UpdateItemPostion(void) {
        int lineHeight = m_lineHeight << 1;
        for (auto& item : m_lrc) {
            item.UpdateSize(lineHeight);
        }
    }

    LrcItem::LrcItem(size_t tm, size_t index, const String content, const RECT& rect)
        : m_tm(tm)
        , m_index(index)
        , m_content(content)
        , m_rect(rect) {

    }

    void LrcItem::Draw(HDC hdc, int cur, const RECT& rect) {
        if (rect.top > m_rect.bottom || rect.bottom < m_rect.top) {
            return;
        }

        if (m_index == cur) {
            COLORREF old = SetTextColor(hdc, RGB(143, 250, 200));
            DrawText(hdc, m_content.c_str(), -1, &m_rect, DT_CENTER);
            SetTextColor(hdc, old);
            return;
        }
        DrawText(hdc, m_content.c_str(), -1, &m_rect, DT_CENTER);
        
    }

    int LrcItem::Compare(const float tm) const {
        if (tm < m_tm) {
            return -1;
        }

        return tm == m_tm ? 0 : 1;
    }

    void LrcItem::UpdateSize(size_t height) {
        m_rect.top -= height;
        m_rect.bottom -= height;
    }

}