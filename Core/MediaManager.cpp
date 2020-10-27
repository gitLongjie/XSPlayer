#include "Core/MediaManager.h"

#include <algorithm>

#include "Core/Media.h"
#include "Core/MediaSource.h"

#include "Request/PlayControlRequest.h"
#include "Request/PlaySeekRequest.h"
#include "Utils.h"
#include "Contanst.h"

namespace XSPlayer {

    template<> MediaManager* Singleton<MediaManager>::m_pSingleton = nullptr;
    std::atomic<size_t> MediaManager::m_meidaCount = 0;

    MediaManager::MediaManager(void) {
        Init();
    }

    MediaManager::~MediaManager(void) {

    }


    void MediaManager::Destory(void) {
        m_listMediaSoruces.clear();
    }

    void MediaManager::SetPlayMedia(size_t meidaID) {
        m_currentPlayMedia = meidaID;
    }

    bool MediaManager::PlayMedia(size_t meidaID) {
        SetPlayMedia(meidaID);
        return PlayMedia();
    }

    bool MediaManager::PlayMedia(void) {
        if (nullptr == m_pMediaHandleChane) {
            return false;
        }

        Media* pMedia = GetMedia(m_currentPlayMedia);
        if (nullptr == pMedia) {
            return nullptr;
        }
        const String& path = pMedia->GetMediaPath();
        PlayControlRequest playControl;
        if (!playControl.Play(path)) {
            return false;
        }

        String name = pMedia->GetText();
        m_pMediaHandleChane->NextHandleRequest(&playControl);
        NotifyEvent(ControlEvent::Create(pMedia, ControlEvent::EControl::EC_PLAY));

//        m_sqliteHelper.UpdateCurrentMedia(path);
        return true;
    }

    bool MediaManager::Pause(void) {
        PlayControlRequest playControl;
        playControl.Pause();
        return m_pMediaHandleChane->NextHandleRequest(&playControl);
    }


    bool MediaManager::Continue(void) {
        PlayControlRequest playControl;
        playControl.Continue();
        return m_pMediaHandleChane->NextHandleRequest(&playControl);
    }

    void MediaManager::Stop(void) {
        if (nullptr == m_pMediaHandleChane) {
            return;
        }
        PlayControlRequest playControl;
        playControl.Stop();
        m_pMediaHandleChane->NextHandleRequest(&playControl);
        m_currentPlayMedia = -1;
    }


    void MediaManager::Seek(long long pos) {
        PlaySeekRequest request(pos, true);
        m_pMediaHandleChane->NextHandleRequest(&request);
    }

    void MediaManager::AddHandleChain(const MediaHandleChainPtr& pHandleChain) {
        m_pMediaHandleChane = pHandleChain;
    }

    bool MediaManager::AddMedia(const Media* pMedia, const String& type) {
        if (nullptr == pMedia) {
            return false;
        }

        Media* pFindMedia = m_pRoot->Find(type);
        if (nullptr == pFindMedia) {
            return false;
        }

        auto pMediaContainer = dynamic_cast<MediaContainer*>(pFindMedia);
        if (nullptr == pMediaContainer) {
            return false;
        }

        return pMediaContainer->Add(const_cast<Media*>(pMedia));
    }

//    bool MediaManager::AddLocalMedia(const String& path) {
//         if (nullptr == m_pLocalMediaContainer) {
//             return false;
//         }
// 
//         String name;
//         if (!m_sqliteHelper.AddLocalMedia(path, name)) {
//             return false;
//         }
// 
//         auto pMediaItem = new MediaItem(name);
//         if (nullptr == pMediaItem) {
//             return false;
//         }
// 
//         pMediaItem->SetMediaPath(path);
//         return m_pLocalMediaContainer->Add(pMediaItem);
//        return false;
//    }


    bool MediaManager::DeleteMedia(unsigned int mediaID) {
        assert(nullptr != m_pRoot);

        return false; //  m_pRoot->Remove(index);
    }

    const String& MediaManager::GetMediaPath(size_t mediaID) const {
        assert(nullptr != m_pRoot);

        Media* pMedia = m_pRoot->Find(mediaID);
        if (nullptr == pMedia) {
            return g_emptyString;
        }

        return pMedia->GetMediaPath();
    }

    String MediaManager::GetMediaName(const size_t mediaID) {
        assert(nullptr != m_pRoot);

        Media* pMedia = m_pRoot->Find(mediaID);
        if (nullptr == pMedia) {
            return g_emptyString;
        }

        return pMedia->GetText();
    }

    Media* MediaManager::GetMedia(const size_t mediaID) {
        assert(nullptr != m_pRoot);

        return m_pRoot->Find(mediaID);
    }

    int MediaManager::GetMediaCount(void) const {
        return static_cast<int>(m_meidaCount.load());
    }


    void MediaManager::SortLocalMediaByNumberName(void) {
//         std::sort(m_mediaFile.begin(), m_mediaFile.end(), [](const auto& i1, const auto& i2) {
//             const String& s1 = i1.first;
//             const String& s2 = i2.first;
// 
//             if (!std::all_of(s1.begin(), s1.end(), std::isdigit) ||
//                 !std::all_of(s2.begin(), s2.end(), std::isdigit)) {
//                 return false;
//             }
// 
//             int ii1 = std::stoi(i1.first);
//             int ii2 = std::stoi(i2.first);
//             return ii1 < ii2;
//         });
    }

    size_t MediaManager::GenerateMeidaId(void) {
        size_t medaiId = m_meidaCount.load();
         ++m_meidaCount;
         return medaiId;
    }


    bool MediaManager::RegistEvent(EventHandle* pEvent) {
        m_listEvent.insert(pEvent);
        return true;
    }

    bool MediaManager::UnregistEvent(EventHandle* pEvent) {
        m_listEvent.erase(pEvent);
        return true;
    }

    bool MediaManager::NotifyEvent(const EventPtr& pEvent) {
        if (nullptr == pEvent || 0 == pEvent->GetID()) {
            return false;
        }

        for (auto& item : m_listEvent) {
            if (item->OnNotify(pEvent)) {
                return true;
            }
        }
        return true;
    }

    bool MediaManager::AddMediaSource(MediaSourceFactory* pMediaSourceFactory,
                                      MediaSourceCallback* pCallback) {
        if (nullptr == pMediaSourceFactory) {
            return false;
        }

        MediaSourceType mediaSourceType = pMediaSourceFactory->GetSourceType();
        if (m_listMediaSoruces.end() != m_listMediaSoruces.find(mediaSourceType)) {
            return false;
        }

        MediaSourcePtr pMediaSource = pMediaSourceFactory->Create();
        if (nullptr == pMediaSource || nullptr == m_pRoot) {
            return false;
        }
        m_listMediaSoruces[mediaSourceType] = pMediaSource;

        pMediaSource->Load(pCallback);
        
        MediaContainer* pMediaContainer = pMediaSource->GetMediaContainer();
        if (nullptr == pMediaContainer) {
            return false;
        }

        return m_pRoot->Add(pMediaContainer);
    }

    void MediaManager::Init(void) {
        m_pRoot = new MediaContainer("root");
    }

}
