#pragma once

//
// DVS declaration
//

#include <filesystem>
#include <string>

#include "common.h"
#include "docopt.h"

enum class SpecialName
{
  DVS,
  BRANCHES_LOCAL,
  BRANCHES_REMOTE,
  INDEX,
  HEAD,
  TAGS,
};

class BaseCommand;

class DVS
{
  public:
  using BasePtr            = std::unique_ptr< BaseCommand >;
  using CommandPointerType = std::function< BasePtr( ) >;
  using CommandMap         = std::map< std::string, CommandPointerType >;

  using ParseResult = struct ParseResult
  {
    bool  executedCommand = false;
    Error errMsg;
  };

  DVS( );
  ~DVS( );

  ParseResult ParseArgs( int argc, char **argv );
  ParseResult ParseArgs( DocOptArgs &, const CommandMap &commandMap_ );

  // Helpers
  Error Validate( const std::string &dir = "" );

  std::filesystem::path GetDvsDirectory( );
  std::filesystem::path GetTopLevelDirectory( );

  bool IsIgnored( const std::filesystem::path & );

  // Read and write the hash id to the .dvs/HEAD file.
  void     SetRef( const std::string &ref, const RefValue &, const bool deref = true );
  RefValue GetRef( const std::string &ref, const bool deref = true );

  Oid GetOid( const std::string &name );

  // Iterate through all references.
  void ForAllRefs( const std::string &                                                   prefix = "",
                   const bool                                                            deref  = true,
                   std::function< void( const std::string &refName, const RefValue & ) > func   = nullptr );

  std::string GetSpecialName( const SpecialName );
  std::filesystem::path GetSpecialPath( const SpecialName );

  protected:
  private:
  using RefIntRet = struct
  {
    std::string ref;
    RefValue    refValue;
  };

  //
  // Data
  //

  std::filesystem::path m_OriginalDirectory;
  std::filesystem::path m_DvsDirectory;

  //
  // Methods
  //

  // Helpers
  std::filesystem::path RemoveLastPathElement( const std::filesystem::path & );
  int                   NumPathElements( const std::filesystem::path & );
  std::string           RelPath( const std::string &filename, const std::string &dirEntry );
  bool                  StartsWith( const std::string &str, const std::string &startsWith );

  RefIntRet GetRefInternal( const std::string &ref, const bool deref );
};
