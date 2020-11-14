#pragma once

//
// DVS declaration
//

#include <filesystem>
#include <string>

#include "common.h"
#include "docopt.h"

// clang-format off
constexpr const char *s_DVS_DIR              = ".dvs";
constexpr const char *s_HEAD_REF             = "HEAD";
constexpr const char *s_REFS_BRANCHES_LOCAL  = "refs/branches-local/";
constexpr const char *s_REFS_BRANCHES_REMOTE = "refs/branches-remote/";
constexpr const char *s_REFS_TAGS            = "refs/tags/";
//clang-format on

class DVS
{
  public:
  DVS( );
  ~DVS( );

  int ParseCommands( int argc, char **argv );

  Error ParseBranchCommands( std::map< std::string, docopt::value > &args_ );
  Error ParseInternalCommands( std::map< std::string, docopt::value > &args_ );
  Error ParseTagCommands( std::map< std::string, docopt::value > &args_ );

  // Helpers
  Error Validate( const std::string &dir = "" );

  std::filesystem::path GetDvsDirectory( );

  bool IsIgnored( const std::filesystem::path & );

  // Read and write the hash id to the .dvs/HEAD file.
  void SetRef( const std::string &ref, const RefValue &, const bool deref = true );
  RefValue GetRef( const std::string &ref, const bool deref = true );

  Oid GetOid( const std::string &name );

  // Iterate through all references.
  void ForAllRefs( const std::string &prefix = "", const bool deref = true, std::function< void ( const std::string &refName, const RefValue & ) > func = nullptr );

  protected:
  private:
  using RefIntRet = struct
  {
    std::string ref;
    RefValue refValue;
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
  int NumPathElements( const std::filesystem::path & );

  RefIntRet GetRefInternal( const std::string &ref, const bool deref );
};
