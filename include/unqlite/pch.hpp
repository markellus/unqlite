#pragma once

#include <cstdlib>
#include <memory>
#include <vector>
#include <tuple>
#include <string>
#include <stdexcept>
#include "core/unqlite.h"

#ifdef UNQLITE_ENABLE_THREADS
#include <mutex>
#include <map>
#define lockIfThreaded() std::scoped_lock lock(m_mutAccess)
#else
#define lockIfThreaded()
#endif

#ifdef MAKEDLL
#  define DLLFUNC __declspec(dllexport)
#else
#  define DLLFUNC __declspec(dllimport)
#endif

namespace mk::unqlitecpp::v1
{
  using mkString = std::string;

  class Database;
  using DatabasePtr = std::shared_ptr<Database>;
  class Query;
  using QueryPtr = std::shared_ptr<Query>;
  template<typename T>
  class QueryResult;
  class CollectionQuery;
  using CollectionQueryPtr = std::shared_ptr<CollectionQuery>;
  class CollectionQueryResult;
  using CollectionQueryResultPtr = std::shared_ptr<CollectionQueryResult>;
}