/**************************************************************************
 Multitouch Software Development Kit (SDK / SOFTWARE)
 eyefactive GmbH, Copyright (C) 2009-2022. All rights reserved.
 ---
 ATTENTION: USE OF THIS SDK IS SUBJECT TO THE EYEFACTIVE SOFTWARE PRODUCT
 "AS IS" WARRANTY STATEMENT BELOW. BY DOWNLOADING AND/OR INSTALLING AND/OR
 USING THE SDK, YOU ARE AGREEING TO BE BOUND BY THESE TERMS.
 IF YOU DO NOT AGREE TO ALL OF THESE TERMS, DO NOT DOWNLOAD AND/OR INSTALL
 AND/OR USE THE SDK ON YOUR SYSTEM.

 EYEFACTIVE SOFTWARE PRODUCT "AS IS" WARRANTY STATEMENT

 DISCLAIMER. TO THE EXTENT ALLOWED BY LOCAL LAW, THIS EYEFACTIVE SOFTWARE
 PRODUCT ("SOFTWARE") IS PROVIDED TO YOU "AS IS" WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, WHETHER ORAL OR WRITTEN, EXPRESS OR IMPLIED.
 EYEFACTIVE SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OR CONDITIONS OF
 MERCHANTABILITY, SATISFACTORY QUALITY, NON-INFRINGEMENT AND FITNESS FOR A
 PARTICULAR PURPOSE.

 LIMITATION OF LIABILITY. EXCEPT TO THE EXTENT PROHIBITED BY LOCAL LAW, IN
 NO EVENT WILL EYEFACTIVE OR ITS SUBSIDIARIES, AFFILIATES OR SUPPLIERS BE
 LIABLE FOR DIRECT, SPECIAL, INCIDENTAL, CONSEQUENTIAL OR OTHER DAMAGES
 (INCLUDING LOST PROFIT, LOST DATA, OR DOWNTIME COSTS), ARISING OUT OF THE
 USE, INABILITY TO USE, OR THE RESULTS OF USE OF THE SOFTWARE, WHETHER BASED
 IN WARRANTY, CONTRACT, TORT OR OTHER LEGAL THEORY, AND WHETHER OR NOT
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. YOUR USE OF THE SOFTWARE IS
 ENTIRELY AT YOUR OWN RISK.
**************************************************************************/

#include "unqlite/Unqlite.hpp"
#include <iostream>
#include <thread>

using namespace mk::unqlitecpp;

int main(int iArgCount, const char** pp_args)
{
  unqlite_lib_config(UNQLITE_LIB_CONFIG_THREAD_LEVEL_MULTI);
  unqlite_lib_init();

  if (!unqlite_lib_is_threadsafe())
  {
    std::string strErr = "UnQLite-cpp: UnQLite was configured without multithreading support at compile time, "
                         "which is strictly required by the c++ API!";
    std::cerr << strErr << std::endl;
    throw std::runtime_error(strErr);
  }

  auto p_db = mk::unqlitecpp::Database::create();

  bool bOpen = p_db->openFile("test.db", false);
  std::cout << bOpen << std::endl;

  auto p_query = p_db->createQuery();
  const char* p_cTest = "rfgwegartbgvb\0";
  std::cout << p_query->store("test", (void*) p_cTest, 14) << std::endl;

  auto p_result = p_query->fetch<char>("test");
  std::cout << p_result->getObjectRef() << std::endl;


  auto p_queryC = p_db->createCollectionQuery(true);

  p_queryC->quickSetup("coll", CollectionQueryType::eCreateCollection);

  std::cout << p_queryC->execute() << std::endl;

  ////////////////////////////////////////

  p_queryC->quickSetup("coll", CollectionQueryType::eFree,
                       "$result ="
                       "{"
                       "  collectionName : $argv[0],"
                       "  args           : $argv[1]"
                       "};");
  for(int i = 0; i < 100000; i++)
  {
    //p_queryC->select(std::to_string(i));
    std::cout << p_queryC->execute("blahhbliblub" + std::to_string(i)) << std::endl;
  }

  ////////////////////////////////////////

  p_queryC->quickSetup("coll", CollectionQueryType::eStore,"");

  for(int i = 0; i < 100000; i++)
  {
    if(p_queryC->execute("{ val: " + std::to_string(i) + "}") == "false")
    {
      return -1;
    }

    if(i % 1000 == 0)
    {
      std::cout << "eStore: " << (i / 100000.0f) << std::endl;
    }

    //p_queryC->commit();
  }

  //p_queryC->commit();

  ////////////////////////////////////////

  p_queryC->quickSetup("coll", CollectionQueryType::eFetch, "");

  for(int i = 0; i < 100000; i++)
  {
    std::string strExpected = "{\"val\":" + std::to_string(i) + ",\"__id\":" + std::to_string(i) + "}";
    std::string strActual = p_queryC->execute(std::to_string(i));
    if(strExpected != strActual)
    {
      std::cout << "eFetch: " << strExpected << "!=" << strActual << std::endl;
      return -1;
    }

    if(i % 1000 == 0)
    {
      std::cout << "eFetch " << (i / 100000.0f) << std::endl;
    }
  }

  ////////////////////////////////////////

  std::thread thr1([&]{
    auto p_queryCT = p_db->createCollectionQuery(true);

    p_queryCT->quickSetup("coll", CollectionQueryType::eIterator,
                         R"--(
  if($record['val'] == $argv[1])
  {
    $result = $record;
    $success = true;
  }
)--");

    for(int i = 0; i < 10000; i++)
    {
      std::string strExpected = "{\"val\":" + std::to_string(i) + ",\"__id\":" + std::to_string(i) + "}";
      std::string strActual = p_queryCT->execute(std::to_string(i));
      if(strExpected != strActual)
      {
        std::cout << "eIterator 1: " << strExpected << "!=" << strActual << std::endl;
        return -1;
      }

      if(i % 100 == 0)
      {
        std::cout << "eIterator 1: " << (i / 10000.0f) << std::endl;
      }
    }

    return 0;
  });

  std::thread thr2([&]{
    auto p_queryCT = p_db->createCollectionQuery(true);

    p_queryCT->quickSetup("coll", CollectionQueryType::eIterator,
                          R"--(
  if($record['val'] == $argv[1])
  {
    $result = $record;
    $success = true;
  }
)--");

    for(int i = 10000; i < 20000; i++)
    {
      std::string strExpected = "{\"val\":" + std::to_string(i) + ",\"__id\":" + std::to_string(i) + "}";
      std::string strActual = p_queryCT->execute(std::to_string(i));
      if(strExpected != strActual)
      {
        std::cout << "eIterator 2: " << strExpected << "!=" << strActual << std::endl;
        return -1;
      }

      if(i % 100 == 0)
      {
        std::cout << "eIterator 2: " << ((i - 10000.0f) / 10000.0f) << std::endl;
      }
    }

    return 0;
  });

  thr1.join();
  thr2.join();

  ////////////////////////////////////////

  p_queryC->quickSetup("coll", CollectionQueryType::eDrop, "");

  for(int i = 0; i < 100000; i++)
  {
    std::string strActual = p_queryC->execute(std::to_string(i));
    if("true" != strActual)
    {
      return -1;
    }

    if(i % 10000 == 0)
    {
      std::cout << "eDrop: " << (i / 100000.0f) << std::endl;
    }
  }

  ////////////////////////////////////////

  p_queryC->quickSetup("coll", CollectionQueryType::eDropCollection, "");
  std::string strActual = p_queryC->execute();

  if("true" != strActual)
  {
    return -1;
  }

  ////////////////////////////////////////

  p_queryC.reset();
  p_db.reset();

  return 0;
}