#ifndef SQLITEHELPER_H
#define SQLITEHELPER_H

#include <vector>

#include "Core/Constant.h"
#include "Core/MediaSource.h"

struct sqlite3;

namespace XSPlayer {
    class SqliteHelper : public MediaSource {
        NON_COPY_ABLE(SqliteHelper);

    public:
        explicit SqliteHelper();
        ~SqliteHelper();

        bool Load(MediaSourceCallback* pCallback) override;
        MediaContainer* GetMediaContainer(void) const override;

        bool AddLocalMedia(const String& szPath, String& name);
        bool DeleteLocalMedia(const String& path);
        bool GetLocalMediasList(MediaContainer* playList);
        void UpdateCurrentMedia(const String& szPath);
        bool GetCurrentMedia(String& szPath);

    private:
        bool OnAddMedia(const String path, const String name);
        bool OnDeleteMedia(const String path);
        bool OnIsHasMedia(const String& szPath);
        void OnUpdateCurrentMedia(const String szPath);

        bool IsHasMediaTable(void) const;
        bool IsHasSettingTalbe(void) const;
        bool IsHasTable(const String& table) const;
        void CreateMediaTable();
        void CreateSettingTable(void);

    private:
        sqlite3* m_pDBHandle = nullptr;
        bool m_bOpen = false;
        MediaContainer* m_pMediaContainer = nullptr;
    };

    class SqliteHelperFactory : public MediaSourceFactory {
    public:
        MediaSourcePtr Create(void) override;
        MediaSourceType GetSourceType(void) const override;
        String GetSourceTypeString(void) const override;
    };
}

#endif  // !SQLITEHELPER_H