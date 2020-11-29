#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <sys/stat.h>

#include "docopt.h"

#include "dvs.h"

#include "CommandParser.h"
#include "command_branch_create.h"
#include "command_branch_delete.h"
#include "command_branch_list.h"
#include "command_branch_switch.h"
#include "command_cat.h"
#include "command_commit.h"
#include "command_diff.h"
#include "command_hash.h"
#include "command_init.h"
#include "command_log.h"
#include "command_read_tree.h"
#include "command_status.h"
#include "command_tag_create.h"
#include "command_tag_delete.h"
#include "command_tag_list.h"
#include "command_unimplemented.h"
#include "command_write_tree.h"

const char s_USAGE[] =
  R"(DVS - David's Versioning System.

    Usage:
      dvs branch create <BranchName>
      dvs branch delete <BranchName>
      dvs branch list
      dvs branch switch <BranchName>
      dvs commit ( -m | --message ) <message>
      dvs diff [<path>]
      dvs fetch
      dvs init [<directory>]
      dvs log [ -p ] [<hash>]
      dvs pull
      dvs push
      dvs status
      dvs tag create <tag> [<hash>]
      dvs tag delete <tag>
      dvs tag list
      dvs internal cat [ -s | -t ] <hash>
      dvs internal hash <file>
      dvs internal read-tree <hash>
      dvs internal write-tree
      dvs (-h | --help)
      dvs --version

    Options:
      -h --help              Show this help information.
      -m, --message          Commit message.
      -p                     Show Patch Diff.
      -s                     Size of hash content.
      -t                     Type of hash content (blob, commit, tag, tree).
      --version              Show version.
)";

// clang-format off
DVS::CommandMap s_MainCommandMap
{
  { "commit", [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< CommitCommand >( );        } },
  { "diff",   [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< DiffCommand >( );          } },
  { "fetch",  [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< UnimplementedCommand >( ); } },
  { "init",   [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< InitCommand >( );          } },
  { "log",    [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< LogCommand >( );           } },
  { "pull",   [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< UnimplementedCommand >( ); } },
  { "status", [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< StatusCommand >( );        } },
};

DVS::CommandMap s_BranchCommandMap
{
  { "create", [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< CreateBranchCommand >( ); } },
  { "delete", [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< DeleteBranchCommand >( ); } },
  { "list",   [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< ListBranchCommand >( );   } },
  { "switch", [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< SwitchBranchCommand >( ); } },
};

DVS::CommandMap s_InternalCommandMap
{
  { "cat",        [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< CatCommand >( ); } },
  { "read-tree",  [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< ReadTreeCommand >( ); } },
  { "hash",       [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< HashCommand >( ); } },
  { "write-tree", [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< WriteTreeCommand >( ); } },
};

DVS::CommandMap s_TagCommandMap
{
  { "create", [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< CreateTagCommand >( ); } },
  { "delete", [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< DeleteTagCommand >( ); } },
  { "list",   [ ]( ) -> std::unique_ptr< BaseCommand > { return std::make_unique< ListTagCommand >( );   } },
};
// clang-format on

using SpecialNameMap = std::map< SpecialName, std::string >;

// clang-format off
SpecialNameMap s_SpecialNameMap
{
  { SpecialName::DVS,             ".dvs"          },
  { SpecialName::BRANCHES_LOCAL,  "refs/locals/"  },
  { SpecialName::BRANCHES_REMOTE, "refs/remotes/" },
  { SpecialName::HEAD,            "HEAD"          },
  { SpecialName::TAGS,            "refs/tags/"    },
};
//clang-format on

DVS::DVS( )
{
  // Save current working directory.
  m_OriginalDirectory = std::filesystem::current_path( );
}

DVS::~DVS( )
{
  // Restore current working directory.
  std::filesystem::current_path( m_OriginalDirectory );
}

DVS::ParseResult DVS::ParseArgs( int argc_, char **argv_ )
{
#ifdef CMAKE_GIT_HASH
#define XSTRINGIFY( s ) STRINGIFY( s )
#define STRINGIFY( s ) #s
  const char *GIT_HASH = XSTRINGIFY( CMAKE_GIT_HASH );
#else
  const char *GIT_HASH = "dev";
#endif

  using DocOptArgs = std::map< std::string, docopt::value >;

  DocOptArgs args = docopt::docopt( s_USAGE,
                                    { argv_ + 1, argv_ + argc_ },
                                    true,                                      // Show help if requested.
                                    std::string( "Version 0.1.0-" ) + GIT_HASH // Version string.
  );

  ParseResult result;

  result = ParseArgs( args, s_MainCommandMap );

  if ( result.executedCommand || !result.errMsg.empty( ) )
  {
    return result;
  }

  if ( docopt::value branchOption = args[ "branch" ]; branchOption && branchOption.isBool( ) && branchOption.asBool( ) )
  {
    result = ParseArgs( args, s_BranchCommandMap );

    if ( result.executedCommand || !result.errMsg.empty( ) )
    {
      return result;
    }
  }

  if ( docopt::value internalOption = args[ "internal" ];
       internalOption && internalOption.isBool( ) && internalOption.asBool( ) )
  {
    result = ParseArgs( args, s_InternalCommandMap );

    if ( result.executedCommand || !result.errMsg.empty( ) )
    {
      return result;
    }
  }

  if ( docopt::value tagOption = args[ "tag" ]; tagOption && tagOption.isBool( ) && tagOption.asBool( ) )
  {
    result = ParseArgs( args, s_TagCommandMap );

    if ( result.executedCommand || !result.errMsg.empty( ) )
    {
      return result;
    }
  }

  result.errMsg = "Command not yet implemented.";

  return result;
}

DVS::ParseResult DVS::ParseArgs( DocOptArgs &args_, const CommandMap &commandMap_ )
{
  ParseResult result;

  for ( auto &myEntry : commandMap_ )
  {
    const std::string &commandName = myEntry.first;

    if ( docopt::value cmdOption = args_[ commandName ]; cmdOption && cmdOption.isBool( ) && cmdOption.asBool( ) )
    {
      BasePtr command = myEntry.second( );
      result.errMsg   = command->ParseArgs( args_ );

      if ( !result.errMsg.empty( ) )
      {
        return result;
      }

      result.errMsg          = command->operator( )( *this );
      result.executedCommand = true;
      return result;
    }
  }

  return result;
}

Error DVS::Validate( const std::string &dir_ )
{
  std::filesystem::path currentPath = m_OriginalDirectory;

  if ( !dir_.empty( ) )
  {
    currentPath = currentPath / dir_;
  }

  for ( ; !currentPath.empty( ); currentPath = RemoveLastPathElement( currentPath ) )
  {
    std::filesystem::path testPath = currentPath / GetSpecialName( SpecialName::DVS );
    if ( std::filesystem::exists( testPath ) )
    {
      m_DvsDirectory = testPath;
      break;
    }
  }

  if ( currentPath.empty( ) )
  {
    std::stringstream ss;
    ss << "This is not a DVS repository because directory " << GetSpecialName( SpecialName::DVS ) << " does not exist.";
    return ss.str( );
  }

  return ""; // No errors.
}

std::filesystem::path DVS::RemoveLastPathElement( const std::filesystem::path &path_ )
{
  int                             num = NumPathElements( path_ );
  std::filesystem::path           newPath;
  std::filesystem::path::iterator itr = path_.begin( );

  for ( int i = 0; i < num - 1; ++i, ++itr )
  {
    newPath /= *itr;
  }

  return newPath;
}

int DVS::NumPathElements( const std::filesystem::path &path_ )
{
  int num = 0;
  for ( std::filesystem::path::iterator itr = path_.begin( ); itr != path_.end( ); ++itr )
  {
    num++;
  }

  return num;
}

std::filesystem::path DVS::GetDvsDirectory( )
{
  return m_DvsDirectory;
}

std::filesystem::path DVS::GetTopLevelDirectory( )
{
  std::filesystem::path dvsDir = GetDvsDirectory( );
  std::filesystem::path topLevelDir = dvsDir.parent_path( );
  return topLevelDir;
}

bool DVS::IsIgnored( const std::filesystem::path &path_ )
{
  const std::string filename = path_.filename( ).string( );
  if ( filename == GetSpecialName( SpecialName::DVS ) )
  {
    return true;
  }

  return false;
}

void DVS::SetRef( const std::string &ref_, const RefValue &refValue_, const bool deref_ )
{
  std::string ref = GetRefInternal( ref_, deref_ ).ref;

  assert( ( (void) "reValue_.value should not be empty.", !refValue_.value.empty( ) ) );

  if ( !m_DvsDirectory.string( ).empty( ) )
  {
    std::string value;

    if ( refValue_.symbolic )
    {
      value = "ref: " + refValue_.value;
    }
    else
    {
      value = refValue_.value;
    }

    std::filesystem::path refPath = m_DvsDirectory;
    refPath /= ref;

    std::filesystem::path dirPath = refPath;
    dirPath                       = dirPath.remove_filename( );
    std::filesystem::create_directories( dirPath );

    std::ofstream headFile( refPath, std::ios_base::binary );

    headFile << value << std::endl;
  }
}

RefValue DVS::GetRef( const std::string &ref_, const bool deref_ )
{
  return GetRefInternal( ref_, deref_ ).refValue;
}

DVS::RefIntRet DVS::GetRefInternal( const std::string &ref_, const bool deref_ )
{
  RefValue headHash;
  bool     symbolic = false;

  if ( !m_DvsDirectory.string( ).empty( ) )
  {
    std::filesystem::path headPath = m_DvsDirectory;

    headPath /= ref_;

    if ( std::filesystem::exists( headPath ) )
    {
      std::ifstream headFile( headPath, std::ios_base::binary );
      std::getline( headFile, headHash.value );
    }

    if ( size_t pos = headHash.value.find( "ref:" ); pos == 0 )
    {
      symbolic = true;

      headHash.value = headHash.value.substr( headHash.value.find( ":" ) + 2 );
      if ( deref_ )
      {
        return GetRefInternal( headHash.value, true );
      }
    }
  }

  return RefIntRet{ ref_, RefValue{ symbolic, headHash.value } };
}

Oid DVS::GetOid( const std::string &name_ )
{
  std::string name = name_ == "@" ? GetSpecialName( SpecialName::HEAD ) : name_;

  Oid oid;

  // clang-format off
  std::vector< std::string > refsToTry
  {
    name,
    "refs/" + name,
    GetSpecialName( SpecialName::TAGS ) + name,
    GetSpecialName( SpecialName::BRANCHES_LOCAL ) + name,
  };
  // clang-format on

  for ( auto &refTry : refsToTry )
  {
    if ( oid = GetRef( refTry, false ).value; !oid.empty( ) )
    {
      break;
    }
  }

  if ( oid.empty( ) )
  {
    oid = name;
  }

  return oid;
}

void DVS::ForAllRefs( const std::string &                                                    prefix_,
                      const bool                                                             deref_,
                      std::function< void( const std::string &refname_, const RefValue & ) > func_ )
{
  std::vector< std::string > refs{ "HEAD" };

  // clang-format off
  std::vector< std::filesystem::path > paths
  {
    GetDvsDirectory( ) / GetSpecialName( SpecialName::BRANCHES_LOCAL ),
    GetDvsDirectory( ) / GetSpecialName( SpecialName::BRANCHES_REMOTE ),
    GetDvsDirectory( ) / GetSpecialName( SpecialName::TAGS ),
  };
  // clang-format on

  for ( auto &rootPath : paths )
  {
    std::error_code ec;
    for ( auto &entry : std::filesystem::directory_iterator( rootPath, ec ) )
    {
      if ( ec )
      {
        continue;
      }

      std::string root =
        RelPath( entry.path( ).string( ), rootPath.parent_path( ).parent_path( ).parent_path( ).string( ) ).substr( 1 );
      refs.push_back( root );
    }
  }

  for ( auto &refname : refs )
  {
    if ( !StartsWith( refname, prefix_ ) )
    {
      continue;
    }

    if ( func_ )
    {
      func_( refname, GetRef( refname, deref_ ) );
    }
  }
}

std::string DVS::RelPath( const std::string &filename_, const std::string &dir_ )
{
  std::string result = filename_;

  if ( StartsWith( filename_, dir_ ) )
  {
    result = filename_.substr( dir_.length( ) );
  }

  return result;
}

bool DVS::StartsWith( const std::string &str_, const std::string &startsWith_ )
{
  return str_.find( startsWith_ ) == 0;
}


std::string DVS::GetSpecialName( const SpecialName name_ )
{
  return s_SpecialNameMap[ name_ ];
}


std::filesystem::path DVS::GetSpecialPath( const SpecialName name_ )
{
  return GetDvsDirectory( ) / GetSpecialName( name_ );
}
