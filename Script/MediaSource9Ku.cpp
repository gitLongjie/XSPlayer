#include "Script/MediaSource9Ku.h"

#include <rapidjson/document.h>

#include "Core/Log.h"
#include "Core/ThreadPool.h"
#include "Core/MediaManager.h"
#include "Script/PyEnvironment.h"
#include "Script/PyModule.h"
#include "Script/PyAgrs.h"
#include "Utils.h"

namespace XSPlayer {

    MediaSource9Ku::MediaSource9Ku() 
        : m_pPyModule(new PyModule("Python.9KuMusic"))
        , m_pMediaContainer(new MediaContainer("serverMediaSource")) {

    }

    MediaSource9Ku::~MediaSource9Ku() {
        if (nullptr != m_pPyModule) {
            delete m_pPyModule;
        }
    }

    bool MediaSource9Ku::Load(MediaSourceCallback* pCallback) {
        TaskPtr pTask = CreateTask(std::bind(&MediaSource9Ku::OnLoad, this,pCallback, shared_from_this()));
        m_threadId = ThreadPool::PushTask(pTask, m_threadId);
        return ThreadPool::IsInvalideThreadId(m_threadId);
    }

    MediaContainer* MediaSource9Ku::GetMediaContainer(void) const {
        return m_pMediaContainer;
    }

    bool MediaSource9Ku::BuilderMediaContents(MediaContainer* pMediaContainer,
                                              MediaSourceCallback* pCallback,
                                              MediaSourceWPtr pWMediaSource) {
        if (nullptr == m_pPyModule || nullptr == pMediaContainer) {
            return false;
        }

        String strContent;
        {
            MediaSourcePtr pThis = pWMediaSource.lock();
            if (!m_pPyModule->CallFunction(strContent, "getMusicContent")) {
                return false;
            }
        }

        return ParseMediaContents(strContent, pMediaContainer, pCallback, pWMediaSource);
    }


    bool MediaSource9Ku::BuilderMediaByType(MediaContainer* pMediaContainer,
                                            MediaSourceCallback* pCallback,
                                            MediaSourceWPtr pWMediaSource) {
        if (nullptr == m_pPyModule || nullptr == pMediaContainer) {
            return false;
        }

        PyAgrs args(1);
        args.Add(pMediaContainer->GetMediaPath().c_str());
        String strContent;
        {
            MediaSourcePtr pThis = pWMediaSource.lock();
            if (nullptr == pThis) {
                return false;
            }

            if (!m_pPyModule->CallFunction(strContent, "getMusicList", args)) {
                return false;
            }
        }
        

        return ParseMediaItems(strContent, pMediaContainer, pCallback, pWMediaSource);
    }

    void MediaSource9Ku::Test() {
        String strContent;
        PyAgrs args(3);
        args.Add("music/t_new.htm");
        args.Add(2);
        args.Add("sss");

        if (!m_pPyModule->CallFunction(strContent, "testArgs", args)) {
            return ;
        }
    }

    bool MediaSource9Ku::ParseMediaContents(const String& content,
                                            MediaContainer* pMediaContainer,
                                            MediaSourceCallback* pCallback,
                                            MediaSourceWPtr pWMediaSource) {
        MediaSourcePtr pThis = pWMediaSource.lock();
        if (nullptr == pMediaContainer || nullptr == pThis) {
            return false;
        }
        rapidjson::Document doc;
        if (doc.Parse(content.c_str()).HasParseError()) {
            LOGE("parse json failed: {}", content);
            return false;
        }

        if (!doc.IsArray()) {
            LOGE("json is not arry");
            return false;
        }

        size_t count = doc.Size();
        for (size_t index = 0; index < count; ++index) {
            const rapidjson::Value& item = doc[index];
            if (item.HasMember("url") && item["url"].IsString() &&
                item.HasMember("type") && item["type"].IsString()) {
                std::string type = Utils::UnicodeToGBK(item["type"].GetString());
                auto pMediaContent = new MediaContainer(type);
                size_t mediaId = MediaManager::GetSingleton().GenerateMeidaId();
                pMediaContent->SetMediaID(mediaId);
                pMediaContent->SetMediaPath(item["url"].GetString());
                pMediaContainer->Add(pMediaContent);
                if (nullptr != pCallback) {
                    pCallback->OnLoadedCallback(pMediaContent);
                }
            }
        }

        return true;
    }

    bool MediaSource9Ku::ParseMediaItems(const String& content,
                                         MediaContainer* pMediaContainer,
                                         MediaSourceCallback* pCallback,
                                         MediaSourceWPtr pWMediaSource) {
        MediaSourcePtr pThis = pWMediaSource.lock();
        if (nullptr == pMediaContainer || nullptr == pThis) {
            return false;
        }

        rapidjson::Document doc;
        if (doc.Parse(content.c_str()).HasParseError()) {
            LOGE("parse json failed: {}", content);
            return false;
        }

        if (!doc.IsArray()) {
            LOGE("json is not arry");
            return false;
        }

        size_t count = doc.Size();
        for (size_t index = 0; index < count; ++index) {
            const rapidjson::Value& item = doc[index];
            if (item.IsString()) {
                std::string url = item.GetString();
                TaskPtr pTask = CreateTask(std::bind(&MediaSource9Ku::OnLoadMedia, this,
                                                     url, pMediaContainer, pCallback,
                                                     shared_from_this()));
                m_threadId = ThreadPool::PushTask(pTask, m_threadId);
            }
        }

        return true;
    }

    bool MediaSource9Ku::ParseMedia(const String& content, MediaContainer* pMediaContainer, MediaSourceCallback* pCallback) {
        if (nullptr == pMediaContainer) {
            return false;
        }

        rapidjson::Document doc;
        if (doc.Parse(content.c_str()).HasParseError()) {
            LOGE("parse json failed: {}", content);
            return false;
        }

        if (doc.HasMember("name") && doc["name"].IsString() &&
            doc.HasMember("singer") && doc["singer"].IsString() &&
            doc.HasMember("url") && doc["url"].IsString()) {
            std::string name = Utils::UnicodeToGBK(doc["name"].GetString());
            std::string singer = Utils::UnicodeToGBK(doc["singer"].GetString());
            std::string url = Utils::UnicodeToGBK(doc["url"].GetString());
            auto pMediaItem = new MediaItem(name);
            size_t mediaId = MediaManager::GetSingleton().GenerateMeidaId();
            pMediaItem->SetMediaID(mediaId);
            pMediaItem->SetArtist(singer);
            pMediaItem->SetMediaPath(url);
            pMediaContainer->Add(pMediaItem);

            if (nullptr != pCallback) {
                pCallback->OnLoadedCallback(pMediaItem);
            }
        }
        return true;
    }

    void MediaSource9Ku::OnLoad(MediaSourceCallback* pCallback, MediaSourceWPtr pWMediaSource) {
        if (nullptr == pWMediaSource.lock()) {
            return;
        }

        if (!BuilderMediaContents(m_pMediaContainer, pCallback, pWMediaSource)) {
            return;
        }

        size_t count = m_pMediaContainer->GetChildernCount();
        for (size_t index = 0; index < count; ++index) {
            auto pMediaContent = dynamic_cast<MediaContainer*>(m_pMediaContainer->GetChild(index));
            if (nullptr == pMediaContent) {
                continue;
            }

            BuilderMediaByType(pMediaContent, pCallback, pWMediaSource);
        }
    }

    void MediaSource9Ku::OnLoadMedia(const String url,
                                     MediaContainer* pMediaContainer,
                                     MediaSourceCallback* pCallback,
                                     MediaSourceWPtr pWMediaSource) {
        MediaSourcePtr pThis = pWMediaSource.lock();
        if (nullptr == pThis) {
            return;
        }

        PyAgrs args(1);
        args.Add(url.c_str());
        String strContent;
        if (!m_pPyModule->CallFunction(strContent, "getMusic", args)) {
            return;
        }

        ParseMedia(strContent, pMediaContainer, pCallback);
    }

    MediaSourcePtr MediaSource9KuFactory::Create(void) {
        return std::make_shared<MediaSource9Ku>();
    }

    MediaSourceType MediaSource9KuFactory::GetSourceType(void) const {
        return MediaSourceType::MST_9KU_SERVER;
    }

    String MediaSource9KuFactory::GetSourceTypeString(void) const {
        return "serverMediaSource";
    }

}
