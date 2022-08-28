#include "unqlite/Database.hpp"
#include "unqlite/Query.hpp"

#define mp_handle static_cast<unqlite*>(mp_db->getHandle())

namespace mk::unqlitecpp::v1
{

  Query::Query(const Database* p_db)
      : mp_db(p_db)
  {

  }

  bool Query::store(const mkString& strKey, void* p_object, int64_t lLength)
  {
    lockIfThreaded();

    if (mp_db->isReadOnly())
    {
      return false;
    }

    return unqlite_kv_store(mp_handle, strKey.c_str(), strKey.size(), p_object, lLength) == UNQLITE_OK;
  }

  void* Query::fetch(const mkString& strKey, int64_t* lLength)
  {
    lockIfThreaded();

    void* p_out = nullptr;

    // According to the unqlite standard, the object must be called twice if you do not know its length.
    // With this extension, the corresponding options are cached so that this is no longer necessary.
    const auto& p_cursor = unqlite_yfext_kv_prefetch(mp_handle, strKey.c_str(), strKey.size(), lLength);

    if (p_cursor)
    {
      p_out = malloc(*lLength);

      if (unqlite_yfext_kv_postfetch(p_cursor, p_out, lLength) == UNQLITE_OK)
      {
        return p_out;
      }
      else
      {
        free(p_out);
      }
    }

    *lLength = 0;
    return nullptr;
  }

  bool Query::drop(const mkString& strKey)
  {
    lockIfThreaded();
    return unqlite_kv_delete(mp_handle, strKey.c_str(), strKey.size()) == UNQLITE_OK;
  }
}