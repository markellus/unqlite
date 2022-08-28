#pragma once

namespace mk::unqlitecpp::v1
{
  enum class CollectionQueryType
  {
      eCreateCollection,
      eDropCollection,
      eStore,
      eFetch,
      eDrop,
      eIterator,
      eFree,
  };

  inline const char* COLLECTION_QUERY_SCRIPTS[] = {
      //////////////////////////////////////
      /// eCreateCollection
      //////////////////////////////////////
      R"=====(
if(!db_exists($argv[0]))
{
  db_create($argv[0]);
  $result = true;
}
else
{
  $result = false;
}
      )=====",
      //////////////////////////////////////
      /// eDropCollection
      //////////////////////////////////////
      R"=====(
$result = db_drop_collection($argv[0]);
      )=====",
      //////////////////////////////////////
      /// eStore
      //////////////////////////////////////
      R"=====(
$json   = json_decode($argv[1]);
$result = false;

if($json == '' || $json == null)
{
  print "Error: JSON-Data is empty!\0";
}
else if(!db_store($argv[0], $json))
{
  print "Error: Object insertion into \"$argv[0]\" failed with: \"$argv[1]\"\0";
}
else
{
  $result = true;
}
      )=====",
      //////////////////////////////////////
      /// eFetch
      //////////////////////////////////////
      R"=====(
$result = db_fetch_by_id($argv[0], $argv[1]);
      )=====",
      //////////////////////////////////////
      /// eDrop
      //////////////////////////////////////
      R"=====(
$result = db_drop_record($argv[0], $argv[1]);
      )=====",
      //////////////////////////////////////
      /// eIterator
      //////////////////////////////////////
      R"=====(
db_reset_record_cursor($argv[0]);

$success     = false;
$multiselect = false;
$result      = [];

while(($record = db_fetch($argv[0])) != NULL)
{
  [FILTER_SCRIPT]

  if($success && !$multiselect)
  {
    break;
  }
}
      )=====",
//      R"=====(
//$success       = false;
//$multiselect   = false;
//$result        = [];
//
//if(count($internalCache) == 0)
//{
//  $zCallback = function($record)
//  {
//    return TRUE;
//  };
//  $internalCache = db_fetch_all($argv[0], $zCallback);
//}
//
//foreach($internalCache as $record)
//{
//  [FILTER_SCRIPT]
//
//  if(!$multiselect && $success)
//  {
//    break;
//  }
//}
//      )=====",
      //////////////////////////////////////
      /// eFree
      //////////////////////////////////////
      R"=====(
      )=====",
  };
}