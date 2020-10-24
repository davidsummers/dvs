#pragma once

//
// DVS declaration
//

#include <string>

constexpr const char *DVS_DIR = ".dvs";

class DVS
{
  public:

    DVS( );
    ~DVS( );

    // Sub-commands.
    std::string Init( );
    std::string Checkout( );
    std::string Commit( );
    std::string Fetch( );
    std::string Pull( );
    std::string Push( );
    std::string Status( );

  protected:

  private:

    std::string Validate( );
};
