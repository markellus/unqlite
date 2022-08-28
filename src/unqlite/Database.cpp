#include "unqlite/Database.hpp"
#include "unqlite/Query.hpp"
#include "unqlite/CollectionQuery.hpp"

namespace mk::unqlitecpp::v1
{
#ifdef UNQLITE_ENABLE_THREADS
  std::mutex gs_mutInstances;
  std::map<mkString, unqlite*> gs_mapInstancesFiles;
  std::map<mkString, unqlite*> gs_mapInstancesMemory;
#endif

  DatabasePtr Database::create()
  {
    return DatabasePtr(new Database());
  }

  Database::Database()
      : m_bOpen(false)
      , m_bReadOnly(false)
      , mp_handle(nullptr)
  {
    static bool gs_bInit = false;

    if(!gs_bInit)
    {
#ifdef UNQLITE_ENABLE_THREADS
      std::scoped_lock lockInstances(gs_mutInstances);
      unqlite_lib_config(UNQLITE_LIB_CONFIG_THREAD_LEVEL_MULTI);
#endif

      unqlite_lib_init();

#ifdef UNQLITE_ENABLE_THREADS
      if (!unqlite_lib_is_threadsafe())
      {
        std::string strErr = "Database: Failed to initialize thread support. This might be a compiler issue.";
        throw std::runtime_error(strErr);
#endif
      }

      gs_bInit = true;
    }
  }

  Database::~Database()
  {
    if (m_bOpen)
    {
      unqlite_close(mp_handle);
    }
  }

  bool Database::openFile(const mkString& strFile, bool bReadOnly)
  {
    if(m_bOpen)
    {
      return false;
    }

#ifdef UNQLITE_ENABLE_THREADS

    std::scoped_lock lockInstances(gs_mutInstances);

    auto iter = gs_mapInstancesFiles.find(strFile);

    if (iter != gs_mapInstancesFiles.end())
    {
      mp_handle = iter->second;
    }

#endif

    lockIfThreaded();

    if (!mp_handle)
    {
      auto iState = unqlite_open(&mp_handle, strFile.c_str(),
                                 bReadOnly ? UNQLITE_OPEN_READONLY : UNQLITE_OPEN_CREATE);

      if (iState != UNQLITE_OK)
      {
        return false;
      }

#ifdef UNQLITE_ENABLE_THREADS
      gs_mapInstancesFiles[strFile] = mp_handle;
#endif
    }

    m_bOpen     = true;
    m_bReadOnly = bReadOnly;
    return true;
  }

  bool Database::openMemory(const mkString& strIdentifier)
  {
    if(m_bOpen)
    {
      return false;
    }

#ifdef UNQLITE_ENABLE_THREADS

    std::scoped_lock lockInstances(gs_mutInstances);

    auto iter = gs_mapInstancesFiles.find(strIdentifier);

    if (iter != gs_mapInstancesMemory.end())
    {
      mp_handle = iter->second;
    }

#endif

    lockIfThreaded();

    if (!mp_handle)
    {
      auto iState = unqlite_open(&mp_handle, strIdentifier.c_str(), UNQLITE_OPEN_IN_MEMORY);

      if (iState != UNQLITE_OK)
      {
        return false;
      }

#ifdef UNQLITE_ENABLE_THREADS
      gs_mapInstancesMemory[strIdentifier] = mp_handle;
#endif
    }

    m_bOpen     = true;
    m_bReadOnly = false;
    return true;
  }

  bool Database::isOpen() const
  {
    return m_bOpen;
  }

  bool Database::isReadOnly() const
  {
    return m_bReadOnly;
  }

  QueryPtr Database::createQuery()
  {
    lockIfThreaded();
    return QueryPtr(new Query(this));
  }

  CollectionQueryPtr Database::createCollectionQuery(bool bUseCache)
  {
    lockIfThreaded();
    return CollectionQueryPtr(new CollectionQuery(this, bUseCache));
  }

  unqlite* Database::getHandle() const
  {
    return mp_handle;
  }
}