#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <sys/stat.h>

#include "docopt.h"

#include "dvs.h"

#include "command_cat.h"
#include "command_checkout.h"
#include "command_commit.h"
#include "command_hash.h"
#include "command_init.h"
#include "command_log.h"
#include "command_read_tree.h"
#include "command_status.h"
#include "command_tag.h"
#include "command_write_tree.h"
#include "CommandParser.h"


const char s_USAGE [] =
R"(DVS - David's Versioning System.

    Usage:
      dvs checkout <hash>
      dvs commit ( -m | --message ) <message>
      dvs fetch
      dvs init [<directory>]
      dvs log [<hash>]
      dvs pull
      dvs push
      dvs status
      dvs tag <tag> [<hash>]

      dvs internal cat [ -s | -t ] <hash>
      dvs internal hash <file>
      dvs internal read-tree <hash>
      dvs internal write-tree

      dvs (-h | --help)
      dvs --version

    Options:
      -h --help              Show this help information.
      -m, --message          Commit message.
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
  std::map< std::string, docopt::value > args =
    docopt::docopt( s_USAGE,
                    { argv_ + 1, argv_ + argc_ },
                    true, // Show help if requested.
                    "dvs Version 1.0" // Version string.
                  );

  std::string err;

  if ( docopt::value checkoutOption = args[ "checkout" ];
       checkoutOption && checkoutOption.isBool( ) && checkoutOption.asBool( ) )
  {
    CheckoutCommand checkoutCmd;

    err = checkoutCmd.ParseArgs( args );

    if ( !err.empty( ) )
    {
      return 1;
    }

    err = checkoutCmd( *this );
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
  else if ( docopt::value fetchOption = args[ "fetch" ];
            fetchOption && fetchOption.isBool( ) && fetchOption.asBool( ) )
  {
    err = "'fetch' subcommand not yet implemented.";
  }
  else if ( docopt::value initOption = args[ "init" ];
            initOption && initOption.isBool( ) && initOption.asBool( ) )
  {
    InitCommand initCommand;

    err = initCommand.ParseArgs( args );

    if ( !err.empty( ) )
    {
      return 1;
    }

    err = initCommand( *this );
  }
  else if ( docopt::value logOption = args[ "log" ];
            logOption && logOption.isBool( ) && logOption.asBool( ) )
  {
    LogCommand logCommand;

    err = logCommand.ParseArgs( args );

    if ( !err.empty( ) )
    {
        return 1;
    }

    err = logCommand( *this );
  }
  else if ( docopt::value pullOption = args[ "pull" ];
            pullOption && pullOption.isBool( ) && pullOption.asBool( ) )
  {
    err = "'pull' subcommand not yet implemented.";
  }
  else if ( docopt::value pushOption = args[ "push" ];
            pushOption && pushOption.isBool( ) && pushOption.asBool( ) )
  {
    err = "'push' subcommand not yet implemented.";
  }
  else if ( docopt::value statusOption = args[ "status" ];
            statusOption && statusOption.isBool( ) && statusOption.asBool( ) )
  {
    StatusCommand statusCommand;

    err = statusCommand( *this );
  }
  else if ( docopt::value tagOption = args[ "tag" ];
            tagOption && tagOption.isBool( ) && tagOption.asBool( ) )
  {
    TagCommand tagCommand;

    err = tagCommand.ParseArgs( args );

    if ( !err.empty( ) )
    {
        return 1;
    }

    err = tagCommand( *this );
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


std::string DVS::ParseInternalCommands( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

  if ( docopt::value catOption = args_[ "cat" ];
       catOption && catOption.isBool( ) && catOption.asBool( ) )
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
  else if ( docopt::value hashOption = args_[ "hash" ];
            hashOption && hashOption.isBool( ) && hashOption.asBool( ) )
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


std::string DVS::Validate( const std::string &dir_ )
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
  int num = NumPathElements( path_ );
  std::filesystem::path newPath;
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
    for ( std::filesystem::path::iterator itr = path_.begin( );
          itr != path_.end( );
          ++itr )
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


void DVS::SetRef( const std::string &ref_, const std::string &hashId_ )
{
  if ( !m_DvsDirectory.string( ).empty( ) )
  {
    std::filesystem::path headPath = m_DvsDirectory;
    headPath /= ref_;

    std::ofstream headFile( headPath, std::ios_base::binary );

    headFile << hashId_ << std::endl;
  }
}


std::string DVS::GetRef( const std::string &ref_ )
{
  std::string headHash;

  if ( !m_DvsDirectory.string( ).empty( ) )
  {
    std::filesystem::path headPath = m_DvsDirectory;

    headPath /= ref_;

    if ( std::filesystem::exists( headPath ) )
    {
      std::ifstream headFile( headPath, std::ios_base::binary );

      headFile >> headHash;
    }
  }

  return headHash;
}
