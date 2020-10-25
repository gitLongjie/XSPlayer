#include "UI/LrcPannel.h"

#include <string>
#include <regex>

#include "Core/MediaManager.h"

namespace XSPlayer {
    String test = "[00:00.00]小建中2019-黄晶富\r\
        [00:00.37]作词：黄晶富\
        [00:01.10]作曲：黄晶富\
        [00:02.09]编曲：黄晶富\
        [00:03.18]统筹：初见文化\
        [00:04.00]出品：听见音乐\
        [00:05.28]发行：锦州听见音乐文化传媒有限公司\
        [00:07.22]发行人：魏来\
\
        [00:21.82]我有一碗小建中\
        [00:23.88]我从来也不喝\
        [00:26.67]有一天我虚劳腹痛\
        [00:29.07]喝完好轻松 Hey\
\
        [00:31.65]小建中汤芍药多\
        [00:34.02]啊比桂枝还多\
        [00:36.55]加上饴糖\
        [00:38.40]这个口感简直嫑太爽\
\
        [00:41.79]你们都说中药苦啊\
        [00:44.12]这就不对啦\
        [00:46.76]小建中汤就很甜啊\
        [00:49.32]还有酸辛咸 Hey\
\
        [00:51.84]中药不苦中医苦啊\
        [00:54.38]中医在哪里\
        [00:56.85]在田野里在大山里\
        [00:59.33]却不在这里\
\
        [01:02.03]哎呀\
        [01:03.21]哎呀呀\
        [01:04.55]我又虚劳腹痛\
\
        [01:07.08]哎呀\
        [01:08.32]哎呀呀\
        [01:09.56]快给我小建中\
\
        [01:12.03]哎呀\
        [01:13.28]哎呀呀\
        [01:14.59]身体快被掏空\
\
        [01:17.17]哎呀\
        [01:18.48]哎呀呀\
        [01:19.62]来一碗小建中";

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

    void LrcPannel::UpdateLrc(size_t tm) {
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

    int LrcPannel::GetCurrentLrc(size_t tm) {
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

    int LrcItem::Compare(const size_t tm) const {
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