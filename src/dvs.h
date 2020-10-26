#pragma once

//
// DVS declaration
//

#include <filesystem>
#include <string>

#include "docopt.h"

constexpr const char *DVS_DIR = ".dvs";

class DVS
{
  public:

    DVS( );
    ~DVS( );

    int ParseCommands( int argc, char **argv );

    std::string ParseInternalCommands( std::map< std::string, docopt::value > &args_ );

    // Helpers
    std::string Validate( );

    std::filesystem::path GetDvsDirectory( );

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
