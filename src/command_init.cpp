#include <filesystem>
#include <iostream>
#include <sstream>

#include "command_init.h"
#include "dvs.h"

std::string InitCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

  if ( docopt::value dirOption = args_[ "<directory>" ];
       dirOption && dirOption.isString( ) && !dirOption.asString( ).empty( ) )
  {
    m_Directory = dirOption.asString( );
  }

  return err;
}


std::string InitCommand::operator ( ) ( DVS &dvs_ )
{
  std::string err = InitDvs( dvs_, m_Directory, &std::cout );

  return err;
}


std::string InitCommand::InitDvs( DVS &dvs_, const std::string rootPath_, std::ostream *outStream_ )
{
  std::string rootPath = rootPath_;

  if ( rootPath.empty( ) )
  {
    rootPath = ".";
  }
 
  // Create initial control directory structure
  if ( rootPath != "." &&
       std::filesystem::exists( rootPath ) &&
       std::filesystem::is_directory( rootPath ) )
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


  if ( std::string validate_error = dvs_.Validate( dvsDir.string( ) );
       !validate_error.empty( ) )
  {
    return validate_error;
  }

  if ( outStream_ != nullptr )
  {
    std::cout << "Initialized empty DVS repository in " <<
      ( rootPath == "." ? "current directory" : std::filesystem::absolute( rootPath ) ) <<
      std::endl;
  }

  return ""; // No error.
}
