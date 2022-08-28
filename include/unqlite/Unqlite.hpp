#pragma once

#ifdef UNQLITE_API_VERSION_EXP

// Reserved for future expansion

#else

#include "unqlite/Database.hpp"
#include "unqlite/Query.hpp"
#include "unqlite/CollectionQuery.hpp"

namespace mk::unqlitecpp
{
  using namespace mk::unqlitecpp::v1;
}

#endif