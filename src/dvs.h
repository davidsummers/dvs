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

    // Sub-commands.
    std::string Init( );
    std::string Checkout( );
    std::string Commit( );
    std::string Fetch( );
    std::string Pull( );
    std::string Push( );
    std::string Status( );

    // Internal commands
    std::string Hash( std::istream &, bool write );

  protected:

  private:

    //
    // Data
    //

    std::filesystem::path m_OriginalDirectory;

    //
    // Methods
    //

    std::string Validate( );
};
