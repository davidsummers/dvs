#include <filesystem>
#include <iostream>
#include <sstream>

#include "command_init.h"
#include "dvs.h"

Error InitCommand::ParseArgs( DocOptArgs &args_ )
{
  std::string err;

  if ( docopt::value dirOption = args_[ "<directory>" ];
       dirOption && dirOption.isString( ) && !dirOption.asString( ).empty( ) )
  {
    m_Directory = dirOption.asString( );
  }

  return err;
}

Error InitCommand::operator( )( DVS &dvs_ )
{
  Error err = InitDvs( dvs_, m_Directory, &std::cout );

  return err;
}

Error InitCommand::InitDvs( DVS &dvs_, const std::string rootPath_, std::ostream *outStream_ )
{
  std::string rootPath = rootPath_;

  if ( rootPath.empty( ) )
  {
    rootPath = ".";
  }

  // Create initial control directory structure
  if ( rootPath != "." && std::filesystem::exists( rootPath ) && std::filesystem::is_directory( rootPath ) )
  {
    std::stringstream ss;
    ss << "Directory '" << rootPath << "' already exists.";
    return ss.str( );
  }

  std::filesystem::path dvsDir = dvs_.GetDvsDirectory( ) / rootPath;
  dvsDir /= s_DVS_DIR;

  // Create DVS directory.
  if ( !std::filesystem::create_directories( dvsDir ) )
  {
    std::stringstream ss;
    ss << "Can't create directory '" << dvsDir << "'";
    return ss.str( );
  }

  // Create DVS/objects directory.
  std::filesystem::path objectsDir = dvsDir;
  objectsDir /= "objects";

  if ( !std::filesystem::create_directory( objectsDir ) )
  {
    std::stringstream ss;
    ss << "Can't create directory '" << objectsDir << "'";
    return ss.str( );
  }

  if ( Error validate_error = dvs_.Validate( dvsDir.string( ) ); !validate_error.empty( ) )
  {
    return validate_error;
  }

  if ( outStream_ != nullptr )
  {
    std::cout << "Initialized empty DVS repository in "
              << ( rootPath == "." ? "current directory" : std::filesystem::absolute( rootPath ) ) << std::endl;
  }

  // Create first (default) branch
  const bool  symbolic    = true;
  std::string newRefValue = s_REFS_BRANCHES_LOCAL;
  newRefValue += "master";
  dvs_.SetRef( s_HEAD_REF, RefValue{ symbolic, newRefValue } );

  return "";
}
