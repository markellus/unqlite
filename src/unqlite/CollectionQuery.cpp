#include "unqlite/Database.hpp"
#include "unqlite/CollectionQuery.hpp"
#include "unqlite/ScriptExtensions.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace mk::unqlitecpp::v1
{
  static int unQLiteScriptDebugOutput(const void* p_out, unsigned int ilength, void* p_data)
  {
    std::string strOut = (const char*) p_out;

    if (strOut.find("[debug]") == 0)
    {
      std::cerr << "yfDatabaseScript: " << strOut.substr(7) << std::endl;
    }
    else
    {
      std::cout << "yfDatabaseScript: " << (const char*) p_out << std::endl;
    }

    return UNQLITE_OK;
  }

  CollectionQuery::CollectionQuery(const Database* p_db, bool bUseCache)
    : mp_db(p_db)
    , m_bUseCache(bUseCache)
    , m_bCompilationRequired(true)
    , mp_vm(nullptr)
  {

  }

  CollectionQuery::~CollectionQuery()
  {
    if(mp_vm)
    {
      unqlite_vm_release(mp_vm);
    }
  }

  void CollectionQuery::quickSetup(const mkString& strCollection, const CollectionQueryType& enumType,
                                   const mkString& strFilterScript)
  {
    select(strCollection);
    setScriptType(enumType);
    setFilterScript(strFilterScript);
  }

  void CollectionQuery::quickSetup(const mkString& strCollection, const CollectionQueryType& enumType)
  {
    select(strCollection);
    setScriptType(enumType);
    setFilterScript("");
  }

  void CollectionQuery::select(const mkString& strCollection)
  {
    lockIfThreaded();
    m_strCollection = strCollection;
    m_bCompilationRequired = true;
  }

  void CollectionQuery::setScriptType(const CollectionQueryType& enumType)
  {
    lockIfThreaded();
    m_enumType = enumType;
    m_bCompilationRequired = true;
  }

  void CollectionQuery::setFilterScript(const mkString& strFilterScript)
  {
    lockIfThreaded();
    switch (m_enumType)
    {
      case CollectionQueryType::eIterator:
      {
        const static std::string c_strFilterPlaceholder = "[FILTER_SCRIPT]";
        m_strFilterScript = COLLECTION_QUERY_SCRIPTS[int(m_enumType)];
        auto iPos = m_strFilterScript.find(c_strFilterPlaceholder);
        m_strFilterScript.replace(iPos, c_strFilterPlaceholder.length(), strFilterScript);
        break;
      }
      case CollectionQueryType::eFree:
      {
        m_strFilterScript = strFilterScript;
        break;
      }
      default:
      {
        m_strFilterScript = COLLECTION_QUERY_SCRIPTS[int(m_enumType)];
      }
    }

    m_bCompilationRequired = true;
  }

  std::string CollectionQuery::execute(const mkString& strArgs)
  {
    lockIfThreaded();
    bool bError = false;

    if(m_bCompilationRequired || !m_bUseCache)
    {
      m_bCompilationRequired = false;

      if (mp_vm)
      {
        unqlite_vm_release(mp_vm);
        mp_vm = nullptr;
      }

      if (unqlite_compile(mp_db->getHandle(), m_strFilterScript.c_str(), m_strFilterScript.size(), &mp_vm) !=
          UNQLITE_OK)
      {
        std::istringstream ssError(getLastError());
        int                iLineError;
        ssError >> iLineError;

        std::cerr << "CollectionQuery::execute: Error while compiling database script!\n"
                  << "    Line Number: " << iLineError << "\n    "
                  << &ssError.str()[std::to_string(iLineError).size() + 1]
                  << "\n -----------------\n -- script source: \n ----------------- \n";

        std::istringstream ssSource(m_strFilterScript);
        std::string        strLine;
        int                iLine = 0;
        while (std::getline(ssSource, strLine))
        {
          std::cerr << std::setfill('0') << std::setw(4) << ++iLine << " | " << strLine << "\n";
        }
        std::cerr << std::endl;

        bError = true;
      }
      else
      {
        unqlite_vm_config(mp_vm, UNQLITE_VM_CONFIG_OUTPUT, &unQLiteScriptDebugOutput, 0);
        ScriptExtensions::install(mp_vm);
      }
    }

    if(!bError)
    {
      unqlite_vm_reset(mp_vm);

      unqlite_vm_config(mp_vm, UNQLITE_VM_CONFIG_ARGV_ENTRY, m_strCollection.c_str());
      unqlite_vm_config(mp_vm, UNQLITE_VM_CONFIG_ARGV_ENTRY, strArgs.c_str());

      if (unqlite_vm_exec(mp_vm) == UNQLITE_OK)
      {
        auto p_result = unqlite_vm_extract_variable(mp_vm, "result");

        if (p_result)
        {
          int  iLength;
          auto p_cResultVal = unqlite_value_to_string(p_result, &iLength);

          return std::string(p_cResultVal, 0, iLength);
        }
      }
    }

    return "[]";
  }

  std::string CollectionQuery::execute()
  {
    return execute("");
  }

  std::string CollectionQuery::getLastError()
  {
    lockIfThreaded();
    const char* p_cBuffer = nullptr;
    int iLength = -1;

    unqlite_config(mp_db->getHandle(), UNQLITE_CONFIG_JX9_ERR_LOG, &p_cBuffer, &iLength);

    return p_cBuffer;
  }

  void CollectionQuery::commit()
  {
    lockIfThreaded();
    if(mp_vm)
    {
      unqlite_vm_release(mp_vm);
      mp_vm = nullptr;
    }

    m_bCompilationRequired = true;

    unqlite_commit(mp_db->getHandle());
  }
}