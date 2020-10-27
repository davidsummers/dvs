#include <iostream>
#include <sstream>

#include "command_write_tree.h"
#include "dvs.h"


std::string WriteTreeCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }

  std::string err = WriteTree( );

  return err;
}


std::string WriteTreeCommand::WriteTree( const std::string &dir_ )
{
  for ( auto const &entry : std::filesystem::directory_iterator( dir_ ) )
  {
    if ( IsIgnored( entry.path( ) ) )
    {
      continue;
    }

    if ( entry.is_regular_file( ) )
    {
      std::cout << "File: " << entry << std::endl;
      // TODO: Do something with the file.
    }
    else if ( entry.is_directory( ) )
    {
      if ( std::string err = WriteTree( entry.path( ).string( ) );
           !err.empty( ) )
      {
        return err;
      }

      // TODO: Do something with the directory entry.
    }
    else
    {
      std::stringstream ss;
      ss << "Unknown file type for " << entry.path( ) << ".";
      return ss.str( );
    }
  }

  return "";
}


bool WriteTreeCommand::IsIgnored( const std::filesystem::path &path_ )
{
  const std::string filename = path_.filename( ).string( );
  if ( filename == DVS_DIR )
  {
    return true;
  }

  return false;
}
