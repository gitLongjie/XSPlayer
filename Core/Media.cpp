#include "Media.h"

#include <assert.h>

#include "Core/Log.h"

namespace XSPlayer {

    Media::Media(Media* pParnet /*= nullptr*/)
        : m_pParent(pParnet) {
        m_path = GetID();
    }

    Media::Media(const String& text, Media* pParnet)
        : m_pParent(pParnet)
        , m_text(text) {
        m_path = GetID();
    }

    Media::~Media(void) {
      
    }

    bool Media::Add(Media* pMedia) {
        pMedia->SetParent(this);
        return true;
    }

    bool Media::Remove(Media* pMedia, bool bDel) {
        pMedia->SetParent(nullptr);
        if (bDel) {
            delete pMedia;
        }
        return true;
    }


    bool Media::Remove(size_t index, bool bDel /*= true*/) {
        UN_USE(index);
        UN_USE(bDel);
        return false;
    }

    void Media::Clear(void) {
    }

    size_t Media::GetAllChildernCount(void) const {
        return 0;
    }

    Media* Media::Find(const String& text) const {
        if (text == m_text) {
            return const_cast<Media*>(this);
        }

        return nullptr;
    }


    Media* Media::Find(const size_t mediaID) const {
        if (mediaID == m_mediaID) {
            return const_cast<Media*>(this);
        }

        return nullptr;
    }

    void Media::SetPath(Media* pMedia) {
        if (nullptr == pMedia) {
            return;
        }

        const String path(GetPath() + pMedia->GetPath());
        pMedia->SetPath(path);
    }

    bool Media::IsInPath(Media* pMedia) const {
        if (nullptr == pMedia) {
            return true;
        }

        String id = pMedia->GetID();
        return String::npos != m_path.find(id);
    }


    String Media::GetID(void) const {
        char temp[20] = { 0 };
        sprintf(temp, "%p", this);
        return temp;
    }

    MediaContainer::MediaContainer(Media* pParnet) : Media(pParnet) {

    }


    MediaContainer::MediaContainer(const String& text, Media* pParnet /*= nullptr*/) : Media(text, pParnet) {

    }

    MediaContainer::~MediaContainer() {

    }

    bool MediaContainer::Add(Media* pMedia) {
        if (nullptr == pMedia) {
            return false;
        }

        if (IsInPath(pMedia)) {
            return false;
        }

        auto itor = std::find(m_listMedias.begin(), m_listMedias.end(), pMedia);
        if (m_listMedias.end() != itor) {
            return false;
        }
        m_listMedias.emplace_back(pMedia);
        SetPath(pMedia);
        return __super::Add(pMedia);

    }

    bool MediaContainer::Remove(Media* pMedia, bool bDel) {
        auto itor = std::find(m_listMedias.begin(), m_listMedias.end(), pMedia);
        if (m_listMedias.end() != itor) {
            m_listMedias.erase(itor);
            return __super::Remove(pMedia, bDel);
        }

        for (const auto& item : m_listMedias) {
            if (item->Remove(pMedia)) {
                return true;
            }
        }
        return false;
    }

    bool MediaContainer::Remove(size_t index, bool bDel /*= true*/) {
        if (index >= m_listMedias.size()) {
            return false;
        }

        return Remove(m_listMedias[index], bDel);
    }

    void MediaContainer::Clear(void) {
        size_t count = m_listMedias.size();
        for (size_t index = 0; index < count; ++index) {
            Media* pMedia = m_listMedias[index];
            pMedia->Clear();
            delete pMedia;
        }

        m_listMedias.clear();
    }

    Media* MediaContainer::Find(const String& text) const{
        Media* pMedia = __super::Find(text);
        if (nullptr != pMedia) {
            return pMedia;
        }

        for (auto pMedia : m_listMedias) {
            pMedia = pMedia->Find(text);
            if (nullptr != pMedia) {
                return pMedia;
            }
        }

        return nullptr;
    }


    Media* MediaContainer::Find(const size_t mediaID) const {
        Media* pMedia = __super::Find(mediaID);
        if (nullptr != pMedia) {
            return pMedia;
        }

        for (auto pMedia : m_listMedias) {
            pMedia = pMedia->Find(mediaID);
            if (nullptr != pMedia) {
                return pMedia;
            }
        }

        return nullptr;
    }

    size_t MediaContainer::GetChildernCount(void) const {
        return m_listMedias.size();
    }


    Media* MediaContainer::GetChild(size_t index) const {
        assert(index < m_listMedias.size());
        return m_listMedias[index];
    }


    size_t MediaContainer::GetAllChildernCount(void) const {
        size_t count = 0;
        for (auto pMedia : m_listMedias) {
            count += pMedia->GetAllChildernCount();
        }

        count += m_listMedias.size();
        return count;
    }

}
