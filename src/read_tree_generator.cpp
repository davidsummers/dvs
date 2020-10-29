#include "command_read_tree.h"
#include "dvs.h"
#include "read_tree_generator.h"


OidResult ReadTreeCommand::ReadTree( DVS &dvs_, const std::string &hashId_ )
{
#if 0
  ReadTreeGenerator readTreeGenerator;

  for ( auto [ err, path, hash ] : readTreeGenerator.GetTree( hashId_, "./" ) )
  {
    std::filesystem::create_directories( path.dirname( ) );
  }
#endif

  return { "Not Yet Implemented.", "" };
}
