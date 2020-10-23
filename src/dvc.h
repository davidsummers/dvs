#ifndef _DVC_H_
#define _DVC_H_
//
// DVC declaration
//

#include <string>

constexpr const char *DVC_DIR = ".dvc";

class DVC
{
  public:

    DVC( );
    ~DVC( );

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

#endif // _DVC_H_
