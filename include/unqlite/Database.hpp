#pragma once

#include "unqlite/pch.hpp"

using unqlite = unqlite;

namespace mk::unqlitecpp::v1
{
  /**
   * The main class to handle a database.
   */
  class Database
  {
    public:

      /**
       * Create a new object, which represents a database that can either be stored on disk, or in memory.
       *
       * Use openFile() or openMemory() to bind the object to a database.
       *
       * Multiple objects for the same database file can be created at any time.
       */
      static DatabasePtr create();

      /**
       * Destroys the object. If the object is bound to a database, it will be automatically closed.
       */
      ~Database();

      /**
       * Opens a database from disk and binds it to this object.
       *
       * @param strFile   The file location of the database.
       * @param bReadOnly If true, the database is read only and can not be changed from this object instance.
       *                  The database can be opened multiple times with this flag set to different values.
       *
       * @return True if the database was opened successfully, otherwise false.
       */
      virtual bool openFile(const mkString& strFile, bool bReadOnly);

      /**
       * Opens a database in memory.
       *
       * @param strIdentifier A string that identifies the database.
       *
       * @return True if the database was opened successfully, otherwise false.
       */
      virtual bool openMemory(const mkString& strIdentifier);

      /**
       * Returns true if a database is opened, or else false.
       */
      virtual bool isOpen() const;

      /**
       * Returns true if the database is opened in read only mode.
       * This value is undefined in case @ref isOpen() returns false.
       */
      virtual bool isReadOnly() const;

      /**
       * Creates a query object, which can be used to access the key/value store of the database.
       */
      virtual QueryPtr createQuery();

      virtual CollectionQueryPtr createCollectionQuery(bool bUseCache);

      /**
       * Returns the database handle. Can be used to acess the C API.
       */
      unqlite* getHandle() const;

    protected:

      Database();

    private:

      bool m_bOpen;
      bool m_bReadOnly;

      /**
       * Internal database handle
       */
      unqlite* mp_handle;

#ifdef UNQLITE_ENABLE_THREADS
      std::mutex m_mutAccess;
#endif

      /**
       * Copying database objects is not supported.
       */
      Database(const Database& db) = delete;
  };
}
