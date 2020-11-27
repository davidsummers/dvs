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
#include "command_hash.h"
#include "command_init.h"
#include "command_log.h"
#include "command_read_tree.h"
#include "command_status.h"
#include "command_tag.h"
#include "command_write_tree.h"

const char s_USAGE[] =
  R"(DVS - David's Versioning System.

    Usage:
      dvs branch create <BranchName>
      dvs branch delete <BranchName>
      dvs branch list
      dvs branch switch <BranchName>
      dvs commit ( -m | --message ) <message>
      dvs fetch
      dvs init [<directory>]
      dvs log [ -p ] [<hash>]
      dvs pull
      dvs push
      dvs status
      dvs tag create <tag> [<hash>]
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

int DVS::ParseCommands( int argc_, char **argv_ )
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
                                    std::string( "Version 0.0.1-" ) + GIT_HASH // Version string.
  );

  Error err;

  if ( docopt::value branchOption = args[ "branch" ]; branchOption && branchOption.isBool( ) && branchOption.asBool( ) )
  {
    err = ParseBranchCommands( args );
  }
  else if ( docopt::value commitCommand = args[ "commit" ];
            commitCommand && commitCommand.isBool( ) && commitCommand.asBool( ) )
  {
    CommitCommand commitCmd;

    err = commitCmd.ParseArgs( args );

    if ( !err.empty( ) )
    {
      return 1;
    }

    err = commitCmd( *this );
  }
  else if ( docopt::value fetchOption = args[ "fetch" ]; fetchOption && fetchOption.isBool( ) && fetchOption.asBool( ) )
  {
    err = "'fetch' subcommand not yet implemented.";
  }
  else if ( docopt::value initOption = args[ "init" ]; initOption && initOption.isBool( ) && initOption.asBool( ) )
  {
    InitCommand initCommand;

    err = initCommand.ParseArgs( args );

    if ( !err.empty( ) )
    {
      return 1;
    }

    err = initCommand( *this );
  }
  else if ( docopt::value logOption = args[ "log" ]; logOption && logOption.isBool( ) && logOption.asBool( ) )
  {
    LogCommand logCommand;

    err = logCommand.ParseArgs( args );

    if ( !err.empty( ) )
    {
      return 1;
    }

    err = logCommand( *this );
  }
  else if ( docopt::value pullOption = args[ "pull" ]; pullOption && pullOption.isBool( ) && pullOption.asBool( ) )
  {
    err = "'pull' subcommand not yet implemented.";
  }
  else if ( docopt::value pushOption = args[ "push" ]; pushOption && pushOption.isBool( ) && pushOption.asBool( ) )
  {
    err = "'push' subcommand not yet implemented.";
  }
  else if ( docopt::value statusOption = args[ "status" ];
            statusOption && statusOption.isBool( ) && statusOption.asBool( ) )
  {
    StatusCommand statusCommand;

    err = statusCommand( *this );
  }
  else if ( docopt::value tagOption = args[ "tag" ]; tagOption && tagOption.isBool( ) && tagOption.asBool( ) )
  {
    err = ParseTagCommands( args );
  }
  else if ( docopt::value internalOption = args[ "internal" ];
            internalOption && internalOption.isBool( ) && internalOption.asBool( ) )
  {
    err = ParseInternalCommands( args );
  }
  else
  {
    std::cerr << "Command not yet implemented." << std::endl;
    return 1;
  }

  if ( !err.empty( ) )
  {
    std::cerr << err << std::endl;
    return 1;
  }

  return 0;
}

Error DVS::ParseBranchCommands( DocOptArgs &args_ )
{
  Error err;

  if ( docopt::value switchOption = args_[ "switch" ];
       switchOption && switchOption.isBool( ) && switchOption.asBool( ) )
  {
    SwitchBranchCommand checkoutCmd;

    err = checkoutCmd.ParseArgs( args_ );

    if ( !err.empty( ) )
    {
      return err;
    }

    err = checkoutCmd( *this );
  }
  else if ( docopt::value createOption = args_[ "create" ];
            createOption && createOption.isBool( ) && createOption.asBool( ) )
  {
    CreateBranchCommand createBranchCommand;

    err = createBranchCommand.ParseArgs( args_ );

    if ( !err.empty( ) )
    {
      return err;
    }

    err = createBranchCommand( *this );
  }
  else if ( docopt::value branchDeleteOption = args_[ "delete" ];
            branchDeleteOption && branchDeleteOption.isBool( ) && branchDeleteOption.asBool( ) )
  {
    DeleteBranchCommand deleteBranchCommand;

    err = deleteBranchCommand.ParseArgs( args_ );

    if ( !err.empty( ) )
    {
      return err;
    }

    err = deleteBranchCommand( *this );
  }
  else if ( docopt::value branchListOption = args_[ "list" ];
            branchListOption && branchListOption.isBool( ) && branchListOption.asBool( ) )
  {
    ListBranchCommand listBranchCommand;

    err = listBranchCommand.ParseArgs( args_ );

    if ( !err.empty( ) )
    {
      return err;
    }

    err = listBranchCommand( *this );
  }

  return err;
}

Error DVS::ParseInternalCommands( DocOptArgs &args_ )
{
  Error err;

  if ( docopt::value catOption = args_[ "cat" ]; catOption && catOption.isBool( ) && catOption.asBool( ) )
  {
    CatCommand catCommand;

    err = catCommand.ParseArgs( args_ );

    if ( !err.empty( ) )
    {
      return err;
    }

    err = catCommand( *this );
  }
  else if ( docopt::value readTreeOption = args_[ "read-tree" ];
            readTreeOption && readTreeOption.isBool( ) && readTreeOption.asBool( ) )
  {
    ReadTreeCommand readTreeCommand;

    err = readTreeCommand.ParseArgs( args_ );

    if ( !err.empty( ) )
    {
      return err;
    }

    err = readTreeCommand( *this );
  }
  else if ( docopt::value hashOption = args_[ "hash" ]; hashOption && hashOption.isBool( ) && hashOption.asBool( ) )
  {
    HashCommand hashCommand;

    err = hashCommand.ParseArgs( args_ );

    if ( !err.empty( ) )
    {
      return err;
    }

    err = hashCommand( *this );
  }
  else if ( docopt::value writeTreeOption = args_[ "write-tree" ];
            writeTreeOption && writeTreeOption.isBool( ) && writeTreeOption.asBool( ) )
  {
    WriteTreeCommand writeTreeCommand;
    err = writeTreeCommand( *this );
  }

  return err;
}

Error DVS::ParseTagCommands( DocOptArgs &args_ )
{
  Error err;

  if ( docopt::value tagCreateOption = args_[ "create" ];
       tagCreateOption && tagCreateOption.isBool( ) && tagCreateOption.asBool( ) )
  {
    TagCommand tagCommand;

    err = tagCommand.ParseArgs( args_ );

    if ( !err.empty( ) )
    {
      return err;
    }

    err = tagCommand( *this );
  }

  return err;
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
    std::filesystem::path testPath = currentPath / s_DVS_DIR;
    if ( std::filesystem::exists( testPath ) )
    {
      m_DvsDirectory = testPath;
      break;
    }
  }

  if ( currentPath.empty( ) )
  {
    std::stringstream ss;
    ss << "Directory " << s_DVS_DIR << " does not exist.";
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

bool DVS::IsIgnored( const std::filesystem::path &path_ )
{
  const std::string filename = path_.filename( ).string( );
  if ( filename == s_DVS_DIR )
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
  std::string name = name_ == "@" ? s_HEAD_REF : name_;

  Oid oid;

  // clang-format off
  std::vector< std::string > refsToTry
  {
    name,
    "refs/" + name,
    s_REFS_TAGS + name,
    s_REFS_BRANCHES_LOCAL + name,
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

  std::vector< std::filesystem::path > paths
  {
    GetDvsDirectory( ) / s_REFS_BRANCHES_LOCAL,
    GetDvsDirectory( ) / s_REFS_BRANCHES_REMOTE,
    GetDvsDirectory( ) / s_REFS_TAGS,
  };

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
