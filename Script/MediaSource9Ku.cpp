#include "Script/MediaSource9Ku.h"

#include <rapidjson/document.h>

#include "Core/Log.h"
#include "Core/ThreadPool.h"
#include "Core/MediaManager.h"
#include "Script/PyEnvironment.h"
#include "Script/PyModule.h"
#include "Script/PyAgrs.h"
#include "Script/LrcEvent.h"
#include "Utils.h"

namespace XSPlayer {

    MediaSource9Ku::MediaSource9Ku() 
        : m_pMediaContainer(new MediaContainer("serverMediaSource")) {
        MediaManager::GetSingleton().RegistEvent(this);
    }

    MediaSource9Ku::~MediaSource9Ku() {
        MediaManager::GetSingleton().UnregistEvent(this);
    }

    bool MediaSource9Ku::Load(MediaSourceCallback* pCallback) {
        TaskPtr pTask = CreateTask(std::bind(&MediaSource9Ku::OnLoad, this,pCallback, shared_from_this()));
        m_threadId = ThreadPool::PushTask(pTask, m_threadId);
        return ThreadPool::IsInvalideThreadId(m_threadId);
    }

    MediaContainer* MediaSource9Ku::GetMediaContainer(void) const {
        return m_pMediaContainer;
    }


    bool MediaSource9Ku::OnNotify(const EventPtr& pEvent) {
        auto pControlEvent = std::dynamic_pointer_cast<ControlEvent>(pEvent);
        if (nullptr == pControlEvent) {
            return false;
        }

        if (ControlEvent::EControl::EC_PLAY == pControlEvent->GetEC()) {
            Media* pMedia = pControlEvent->GetMedia();
            if (nullptr == pMedia) {
                return false;
            }

            TaskPtr pTask = CreateTask(std::bind(&MediaSource9Ku::OnLoadLrcContent, this, pMedia->GetMediaPath()));
            ThreadPool::PushTask(pTask, ThreadPool::InvalidThreadId());
            return false;
        }
        return true;
    }

    bool MediaSource9Ku::BuilderMediaByType(MediaContainer* pMediaContainer,
                                            MediaSourceCallback* pCallback,
                                            PyModule* pPyModule) {
        if (nullptr == pPyModule || nullptr == pMediaContainer) {
            return false;
        }

        PyAgrs args(1);
        args.Add(pMediaContainer->GetMediaPath().c_str());
        String strContent;

        if (!pPyModule->CallFunction(strContent, "getMusicList", args)) {
            return false;
        }
       
        return ParseMediaItems(strContent, pMediaContainer, pCallback);
    }

    void MediaSource9Ku::Test() {
        String strContent;
        PyAgrs args(3);
        args.Add("music/t_new.htm");
        args.Add(2);
        args.Add("sss");
// 
//         if (!m_pPyModule->CallFunction(strContent, "testArgs", args)) {
//             return ;
//         }
    }

    bool MediaSource9Ku::ParseMediaContents(const String& content,
                                            MediaContainer* pMediaContainer,
                                            MediaSourceCallback* pCallback) {
        if (nullptr == pMediaContainer) {
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
                                         MediaSourceCallback* pCallback) {
        if (nullptr == pMediaContainer) {
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
            if (item.IsInt()) {
                int mediaID = item.GetInt();
                TaskPtr pTask = CreateTask(std::bind(&MediaSource9Ku::OnLoadMedia, this,
                                           mediaID, pMediaContainer, pCallback,
                                           shared_from_this()));
                ThreadPool::PushTask(pTask, ThreadPool::InvalidThreadId());
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

        PyThreadContext* pyThreadContext = PyEnvironment::Create();

        PyModule pPyModule("Python.9KuMusic");

        String strContent;
        if (!pPyModule.CallFunction(strContent, "getMusicContent")) {
            PyEnvironment::ReleaseThreadContext(pyThreadContext);
            return;
        }

        if (!ParseMediaContents(strContent, m_pMediaContainer, pCallback)) {
            PyEnvironment::ReleaseThreadContext(pyThreadContext);
            return;
        }

        size_t count = m_pMediaContainer->GetChildernCount();
        for (size_t index = 0; index < count; ++index) {
            auto pMediaContent = dynamic_cast<MediaContainer*>(m_pMediaContainer->GetChild(index));
            if (nullptr == pMediaContent) {
                continue;
            }

            BuilderMediaByType(pMediaContent, pCallback, &pPyModule);
        }
        PyEnvironment::ReleaseThreadContext(pyThreadContext);
    }

    void MediaSource9Ku::OnLoadMedia(const int songID,
                                     MediaContainer* pMediaContainer,
                                     MediaSourceCallback* pCallback,
                                     MediaSourceWPtr pWMediaSource) {
        
        MediaSourcePtr pThis = pWMediaSource.lock();
        if (nullptr == pThis) {
            return;
        }

        PyThreadContext* pContext = PyEnvironment::Create();
        std::shared_ptr<PyModule> pLoadMedia = std::make_shared<PyModule>("Python.9KuSong");
        PyAgrs args(1);
        args.Add(songID);
        String strContent;
        if (!pLoadMedia->CallFunction(strContent, "getMusicInfo", args)) {
            PyEnvironment::ReleaseThreadContext(pContext);
            return;
        }

        args.Destroy();
        ParseMedia(strContent, pMediaContainer, pCallback);
        PyEnvironment::ReleaseThreadContext(pContext);
    }

    void MediaSource9Ku::OnLoadLrcContent(const String mediaPath) {
        PyThreadContext* pContext = PyEnvironment::Create();
        std::shared_ptr<PyModule> pLoadMedia = std::make_shared<PyModule>("Python.9KuSongLrc");
        PyAgrs args(1);
        args.Add(mediaPath.c_str());
        String strContent;
        if (!pLoadMedia->CallFunction(strContent, "getMusicLrc", args)) {
            PyEnvironment::ReleaseThreadContext(pContext);
            return;
        }
        args.Destroy();

        std::string strLrc = Utils::UnicodeToGBK(strContent);
        MediaManager::GetSingleton().NotifyEvent(LrcEvent::Create(strLrc));
        PyEnvironment::ReleaseThreadContext(pContext);
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
