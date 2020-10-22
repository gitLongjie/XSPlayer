#ifndef MEDIA_H
#define MEDIA_H

#include <memory>
#include <string>

#include "Core/Constant.h"

namespace XSPlayer {

    class Media {
        NON_COPY_ABLE(Media)

    public:
        Media(Media* pParnet = nullptr);
        Media(const String& text, Media* pParnet = nullptr);
        virtual ~Media(void);

    public:
        virtual bool Add(Media* pMedia);
        virtual bool Remove(Media* pMedia, bool bDel = true);
        virtual bool Remove(size_t index, bool bDel = true);
        virtual void Clear(void);
        virtual bool IsLeaf(void) const { return true; }
        virtual size_t GetAllChildernCount(void) const;
        virtual Media* Find(const String& text) const;
        virtual Media* Find(const size_t mediaID) const;

        void SetParent(Media* pParent) { m_pParent = pParent; }
        Media* GetParent(void) const { return m_pParent; }
        void SetText(const String& text) { m_text = text; }
        const String& GetText(void) const { return m_text; }
        void SetMediaPath(const String& path) { m_mediaPath = path; }
        const String& GetMediaPath(void) const { return m_mediaPath; }
        void SetUserData(void* pData) { m_pUserData = pData; }
        void* GetUserData(void) const { return m_pUserData; }
        void SetMediaID(unsigned int mediaID) { m_mediaID = mediaID; }
        unsigned int GetMediaId(void) const { return m_mediaID; }

    protected:
        void SetPath(const String& path) { m_path = path; }
        void SetPath(Media* pMedia);
        const String& GetPath(void) const { return m_path; }
        bool IsInPath(Media* pMedia) const;
        
    private:
        String GetID(void) const;

    private:
        String m_path;
        Media* m_pParent = nullptr;
        String m_text;
        String m_mediaPath;
        void* m_pUserData = nullptr;
        unsigned int m_mediaID = 0;
    };

    class MediaContainer : public Media {
        NON_COPY_ABLE(MediaContainer)
    public:
        MediaContainer(Media* pParnet = nullptr);
        MediaContainer(const String& text, Media* pParnet = nullptr);
        ~MediaContainer() override;

    public:
        bool Add(Media* pMedia) override;
        bool Remove(Media* pMedia, bool bDel = true) override;
        bool Remove(size_t index, bool bDel = true) override;
        size_t GetAllChildernCount(void) const override;
        void Clear(void) override;
        Media* Find(const String& text) const override;
        Media* Find(const size_t mediaID) const override;
        virtual bool IsLeaf(void) const { return false; }

    public:
        size_t GetChildernCount(void) const;
        Media* GetChild(size_t index) const;

    private:
        using Medias = std::vector<Media*>;
        Medias m_listMedias;
    };
}

#endif // MEDIA_H
