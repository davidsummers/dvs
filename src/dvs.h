#pragma once

//
// DVS declaration
//

#include <filesystem>
#include <string>

constexpr const char *DVS_DIR = ".dvs";

class DVS
{
  public:

    DVS( );
    ~DVS( );

    int ParseCommands( int argc, char **argv );

    // Sub-commands.
    std::string Init( );
    std::string Checkout( );
    std::string Commit( );
    std::string Fetch( );
    std::string Hash( std::istream &, bool write );
    std::string Pull( );
    std::string Push( );
    std::string Status( );

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
