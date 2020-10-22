#include "SqliteHelper.h"

#include <sqlite3.h>

#include "Core/Log.h"
#include "Core/ThreadPool.h"
#include "Core/Media.h"
#include "Core/MediaItem.h"
#include "Core/MediaManager.h"
#include "Utils.h"
#include "Contanst.h"

namespace XSPlayer {

    // 编译环境：VS 2008+sp1，Win7, 工程设置为多字节字符集
    std::string MbcsToUtf8( const char* pszMbcs ) {
        std::string str;
        WCHAR   *pwchar=0;
        CHAR    *pchar=0;
        int len=0;
        int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
        len=MultiByteToWideChar(codepage, 0, pszMbcs, -1, NULL,0);
        pwchar=new WCHAR[len];
        if(pwchar!=0)  {
            len = MultiByteToWideChar(codepage, 0, pszMbcs, -1, pwchar, len);
            if( len!=0 )   {
                len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, 0, 0, 0, 0);
                pchar=new CHAR[len];    
                if(pchar!=0) {
                    len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, pchar, len,0, 0);
                    if(len!=0) {
                        str = pchar;
                    }
                    delete[] pchar; 
                }
                delete[] pwchar;
            }
        }
        return str;
    }

    SqliteHelper::SqliteHelper()
        : m_pMediaContainer(new MediaContainer("localMediaSource")) {
        String strPath = Utils::GetAppPath();
        strPath.append(_T("/sqlite.db"));
#ifdef _UNICODE
        int ret = sqlite3_open(MbcsToUtf8(Utils::WStringToString(strPath).c_str()).c_str(), &m_pDBHandle);
#else
        int ret = sqlite3_open(MbcsToUtf8(strPath.c_str()).c_str(), &m_pDBHandle);
#endif
        
        if (SQLITE_OK != ret) {
            LOGE("open sqllit is failed");
            return;
        }
         m_bOpen = true;
    }

    SqliteHelper::~SqliteHelper() {
        if (nullptr != m_pDBHandle) {
            sqlite3_close(m_pDBHandle);
        }
    }

    bool SqliteHelper::Load(MediaSourceCallback* pCallback) {
        if (!m_bOpen) {
            return false;
        }

        const char* sql = "select media_name, media_path from media;";

        char** table = nullptr;
        int row = 0;
        int col = 0;
        char* szError = nullptr;
        int ret = sqlite3_get_table(m_pDBHandle, sql, &table, &row, &col, &szError);
        if (SQLITE_OK != ret) {
            LOGE("{}", szError);
            sqlite3_free(szError);
            return false;
        }

        for (int r = 2; r <= row * 2; r += 2) {

#ifdef _UNICODE
            String name(Utils::StringToWString(table[r]));
            String path(Utils::StringToWString(table[r + 1]));
            auto meidaFile = std::make_pair(name, path);
            pLocalMediaContainer.emplace_back(meidaFile);
#else
            String name(table[r]);
            String path(table[r + 1]);

            size_t mediaID = MediaManager::GetSingleton().GenerateMeidaId();
            auto pMediaItem = new MediaItem(name);
            pMediaItem->SetMediaID(mediaID);
            pMediaItem->SetMediaPath(path);
            m_pMediaContainer->Add(pMediaItem);

            if (nullptr != pCallback) {
                pCallback->OnLoadedCallback(pMediaItem);
            }
#endif
        }

        sqlite3_free_table(table);
        return true;
    }

    MediaContainer* SqliteHelper::GetMediaContainer(void) const {
        return m_pMediaContainer;
    }

    bool SqliteHelper::AddLocalMedia(const String& szPath, String& name) {
        if (!m_bOpen) {
            return false;
        }

        if (!Utils::GetFileNameByPath(szPath, name)) {
            return false;
        }

        TaskPtr pTask = CreateTask(std::bind(&SqliteHelper::OnAddMedia, this, szPath, name));
        return ThreadPool::InvalidThreadId() != ThreadPool::GetSingleton().PushTask(pTask,
                                                                                    std::thread::id(),
                                                                                    0,
                                                                                    ThreadType::TT_IO);
    }

    bool SqliteHelper::DeleteLocalMedia(const String& path) {
        TaskPtr pTask = CreateTask(std::bind(&SqliteHelper::OnDeleteMedia, this, path));
        return ThreadPool::InvalidThreadId() != ThreadPool::GetSingleton().PushTask(pTask,
                                                                                    std::thread::id(),
                                                                                    0,
                                                                                    ThreadType::TT_IO);
    }

    bool SqliteHelper::GetLocalMediasList(MediaContainer* pLocalMediaContainer) {
        if (!m_bOpen) {
            return false;
        }

        const char* sql = "select media_id, media_name, media_path from media;";

        char** table = nullptr;
        int row = 0;
        int col = 0;
        char* szError = nullptr;
        int ret = sqlite3_get_table(m_pDBHandle, sql, &table, &row, &col, &szError);
        if (SQLITE_OK != ret){
            LOGE("{}", szError);
            sqlite3_free(szError);
            return false;
        }

        for (int r = 2; r <= row*2; r+=2) {
            
#ifdef _UNICODE
            unsigned int mediaID = std::stoul(Utils::StringToWString(table[r]));
            String name(Utils::StringToWString(table[r + 1]));
            String path(Utils::StringToWString(table[r + 2]));
            auto meidaFile = std::make_pair(name, path);
            pLocalMediaContainer.emplace_back(meidaFile);
#else
            unsigned int mediaID = std::stoul(table[r]);
            String name(table[r + 1]);
            String path(table[r + 2]);

            auto pMediaItem = new MediaItem(name);
            pMediaItem->SetMediaID(mediaID);
            pMediaItem->SetMediaPath(path);
            pLocalMediaContainer->Add(pMediaItem);
#endif
        }
        sqlite3_free_table(table);
        return true;
    }

    void SqliteHelper::UpdateCurrentMedia(const String& szPath) {
        TaskPtr pTask = CreateTask(std::bind(&SqliteHelper::OnUpdateCurrentMedia, this, szPath));
        ThreadPool::GetSingleton().PushTask(pTask, std::thread::id(),
                                            0,
                                            ThreadType::TT_IO);
    }

    bool SqliteHelper::GetCurrentMedia(String& szPath) {
        if (!m_bOpen) {
            return false;
        }

        if (!IsHasSettingTalbe()) {
            CreateSettingTable();
            if (!IsHasSettingTalbe()) {
                return false;
            }
        }

        const char* sql = "select cur_media from setting where id = 0;";

        char** table = nullptr;
        int row = 0;
        int col = 0;
        char* szError = nullptr;
        int ret = sqlite3_get_table(m_pDBHandle, sql, &table, &row, &col, &szError);
        if (SQLITE_OK != ret) {
            LOGE("{}", szError);
            sqlite3_free(szError);
            return false;
        }

        if (row <= 0) {
            return false;
        }

#ifdef _UNICODE
        szPath = Utils::StringToWString(table[1]);
#else
        szPath = table[1];
#endif
        sqlite3_free_table(table);
        return true;
    }

    bool SqliteHelper::OnAddMedia(const String path, const String name) {
        if (!m_bOpen) {
            return false;
        }

        if (!IsHasMediaTable()) {
            CreateMediaTable();
        }

        if (OnIsHasMedia(path)) {
            return true;
        }

       
        String sql = _T("insert into media(media_name, media_path) values('");// "select count(*) from media where path='" + szPath + "'";
        sql += name;
        sql += _T("','");

        sql += path;
        sql += _T("')");

        char* szError = nullptr;
#ifdef _UNICODE
        int ret = sqlite3_exec(m_pDBHandle, Utils::WStringToString(sql).c_str(), nullptr, 0, &szError);
#else
        int ret = sqlite3_exec(m_pDBHandle, sql.c_str(), nullptr, 0, &szError);
#endif
        if (ret != SQLITE_OK) {
            LOGE("insert sql failed: {}", szError);
            sqlite3_free(szError);
            return false;
        }

        return true;
    }

    bool SqliteHelper::OnDeleteMedia(const String path) {
        if (!m_bOpen) {
            return false;
        }

        String sql = _T("delete from media where media_path='");
        sql += path;
        sql += _T("';");

        char* szError = nullptr;
#ifdef _UNICODE
        int ret = sqlite3_exec(m_pDBHandle, Utils::WStringToString(sql).c_str(), nullptr, 0, &szError);
#else
        int ret = sqlite3_exec(m_pDBHandle, sql.c_str(), nullptr, 0, &szError);
#endif
        if (ret != SQLITE_OK) {
            LOGE("delete sql failed: {}", szError);
            sqlite3_free(szError);
            return false;
        }

        return true;
    }

    bool SqliteHelper::OnIsHasMedia(const String& szPath) {
        if (!m_bOpen) {
            return false;
        }

        sqlite3_stmt* pStmt = nullptr;
        String sql = _T("select count(*) from media where path='") + szPath + _T("'");
#ifdef _UNICODE
        sqlite3_prepare(m_pDBHandle, Utils::WStringToString(sql).c_str(), sql.length(), &pStmt, nullptr);
#else
        sqlite3_prepare(m_pDBHandle, sql.c_str(), sql.length(), &pStmt, nullptr);
#endif
        sqlite3_step(pStmt);
        int count = sqlite3_column_int(pStmt, 0);
        sqlite3_finalize(pStmt);

        return count > 0;
    }

    void SqliteHelper::OnUpdateCurrentMedia(const String szPath) {
        if (!m_bOpen) {
            return;
        }

        if (!IsHasSettingTalbe()) {
            CreateSettingTable();
            if (!IsHasSettingTalbe()) {
                return;
            }
        }

        String sql = _T("update setting set cur_media='");
        sql += szPath;
        sql += _T("' where id = 0");

        char* szError = nullptr;
#ifdef _UNICODE
        int ret = sqlite3_exec(m_pDBHandle, Utils::WStringToString(sql).c_str(), nullptr, 0, &szError);
#else
        int ret = sqlite3_exec(m_pDBHandle, sql.c_str(), nullptr, 0, &szError);
#endif
        if (ret != SQLITE_OK) {
            LOGE("update current media failed: {}", szError);
            sqlite3_free(szError);
            return ;
        }
    }

    bool SqliteHelper::IsHasMediaTable(void) const {
        if (!m_bOpen) {
            return false;
        }

        return IsHasTable(_T("media"));
    }

    bool SqliteHelper::IsHasSettingTalbe(void) const {
        if (!m_bOpen) {
            return false;
        }

        return IsHasTable(_T("setting"));
    }

    bool SqliteHelper::IsHasTable(const String& table) const {
        sqlite3_stmt* pStmt = nullptr;
        String sql = _T("select count(*) from sqlite_master where type='table' AND name = '") + table + _T("'");
#ifdef _UNICODE
        sqlite3_prepare(m_pDBHandle, Utils::WStringToString(sql).c_str(), sql.length(), &pStmt, nullptr);
#else
        sqlite3_prepare(m_pDBHandle, sql.c_str(), sql.length(), &pStmt, nullptr);
#endif
        sqlite3_step(pStmt);
        int count = sqlite3_column_int(pStmt, 0);
        sqlite3_finalize(pStmt);
        return count > 0;
    }

    void SqliteHelper::CreateMediaTable() {
        if (!m_bOpen) {
            return;
        }

        const char* sql = "CREATE TABLE [media] ([media_id] INTEGER  NOT NULL PRIMARY KEY,\
            [media_name] VARCHAR(256)  NULL, [media_path] VARCHAR(256)  UNIQUE NOT NULL)";
        char* szError = nullptr;
        int ret = sqlite3_exec(m_pDBHandle, sql, nullptr, 0, &szError);
        if (ret != SQLITE_OK) {
            LOGE("insert sql failed: {}", sql);
        }

    }

    void SqliteHelper::CreateSettingTable(void) {
        if (!m_bOpen) {
            return;
        }

        const char* sql = "CREATE TABLE [setting] ( \
            [id] INTEGER  NOT NULL PRIMARY KEY, \
            [cur_media] VARCHAR(200)  NULL )";
        char* szError = nullptr;
        int ret = sqlite3_exec(m_pDBHandle, sql, nullptr, 0, &szError);
        if (ret != SQLITE_OK) {
            LOGE("insert sql failed: {}", sql);
            return;
        }

        const char* insertSql = "insert into setting(id, cur_media) values('0', 'unknown');";

        szError = nullptr;
        ret = sqlite3_exec(m_pDBHandle, insertSql, nullptr, 0, &szError);
        if (ret != SQLITE_OK) {
            LOGE("insert sql failed: {}", szError);
            sqlite3_free(szError);
            return ;
        }
    }

    MediaSourcePtr SqliteHelperFactory::Create(void) {
        return std::make_shared<SqliteHelper>();
    }

    MediaSourceType SqliteHelperFactory::GetSourceType(void) const {
        return MediaSourceType::MST_LOCAL;
    }

    String SqliteHelperFactory::GetSourceTypeString(void) const {
        return kOfflineSource;
    }

}