#pragma once

namespace mk::unqlitecpp::v1
{
  template<typename T>
  class QueryResult
  {
    public:
      ~QueryResult()
      {
        free(mp_object);
      }

      T* getObjectRef()
      {
        return mp_object;
      }

      T* getObjectCopy()
      {
        T* p_copy = std::malloc(sizeof(T));
        memcpy(p_copy, mp_object, sizeof(T));
        return p_copy;
      }

      std::shared_ptr<T> createSharedCopy()
      {
        return std::shared_ptr<T>(getObjectCopy(), [](T* p) { free(p); });
      }

    protected:

      QueryResult(T* p_object)
          : mp_object(p_object) {}

    private:

      T* mp_object;

      friend class Query;

      QueryResult(const QueryResult& qr) = delete;
  };
}