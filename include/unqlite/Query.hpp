#pragma once

#include "unqlite/pch.hpp"
#include "unqlite/QueryResult.hpp"

namespace mk::unqlitecpp::v1
{
  template<typename T>
  class QueryResult;

  class Query
  {
    public:

      template<typename T>
      bool store(const mkString& strKey, T* p_object)
      {
        return store(strKey, p_object, sizeof(T));
      }

      virtual bool store(const mkString& strKey, void* p_object, int64_t lLength);

      template<typename T>
      std::shared_ptr<QueryResult<T>> fetch(const mkString& strKey)
      {
        int64_t lLength;
        void* p_object = fetch(strKey, &lLength);

        if(p_object)
        {
          return std::shared_ptr<QueryResult<T>>(new QueryResult<T>(static_cast<T*>(p_object)));
        }

        return nullptr;
      }

      virtual void* fetch(const mkString& strKey, int64_t* lLength);

      virtual bool drop(const mkString& strKey);

    protected:

      explicit Query(const Database* p_db);

    private:

      const Database* mp_db;

#ifdef UNQLITE_ENABLE_THREADS
      std::mutex m_mutAccess;
#endif

      friend class Database;
  };
}