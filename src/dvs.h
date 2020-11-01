#pragma once

//
// DVS declaration
//

#include <filesystem>
#include <string>

#include "docopt.h"

constexpr const char *s_DVS_DIR = ".dvs";
constexpr const char *s_HEAD_REF = "HEAD";

class DVS
{
  public:

    DVS( );
    ~DVS( );

    int ParseCommands( int argc, char **argv );

    std::string ParseInternalCommands( std::map< std::string, docopt::value > &args_ );

    // Helpers
    std::string Validate( const std::string &dir = "" );

    std::filesystem::path GetDvsDirectory( );

    bool IsIgnored( const std::filesystem::path & );

    // Read and write the hash id to the .dvs/HEAD file.
    void SetRef( const std::string &ref, const std::string &hashId );
    std::string GetRef( const std::string &ref );

    std::string GetOid( const std::string &name );

  protected:

  private:

    //
    // Data
    //

    std::filesystem::path m_OriginalDirectory;
    std::filesystem::path m_DvsDirectory;

    // Helpers
    std::filesystem::path RemoveLastPathElement( const std::filesystem::path & );
    int NumPathElements( const std::filesystem::path & );

    //
    // Methods
    //
};
