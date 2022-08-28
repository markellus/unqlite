#pragma once

#include "unqlite/pch.hpp"
#include "unqlite/CollectionQueryType.hpp"

using unqlite_vm = unqlite_vm;

namespace mk::unqlitecpp::v1
{
  class Database;

  class CollectionQuery
  {
    public:

      ~CollectionQuery();

      void quickSetup(const mkString& strCollection, const CollectionQueryType& enumType,
                      const mkString& strFilterScript);

      void quickSetup(const mkString& strCollection, const CollectionQueryType& enumType);

      virtual void select(const mkString& strCollection);

      virtual void setScriptType(const CollectionQueryType& enumType);

      virtual void setFilterScript(const mkString& strFilterScript);

      virtual std::string execute(const mkString& strArgs);
      std::string execute();

      virtual std::string getLastError();

      virtual void commit();

    protected:

      CollectionQuery(const Database* p_db, bool bUseCache);

    private:

      const Database* mp_db;

      const bool          m_bUseCache;
      mkString            m_strCollection;
      CollectionQueryType m_enumType;
      mkString            m_strFilterScript;
      bool                m_bCompilationRequired;

      unqlite_vm* mp_vm;

#ifdef UNQLITE_ENABLE_THREADS
      std::mutex m_mutAccess;
#endif

      friend class Database;
      CollectionQuery(const CollectionQuery&) = delete;
  };
}