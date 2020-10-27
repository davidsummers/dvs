#include <iostream>
#include <sstream>

#include "command_hash.h"
#include "command_write_tree.h"
#include "dvs.h"


std::string WriteTreeCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }

  std::string err = WriteTree( dvs_ );

  return err;
}


std::string WriteTreeCommand::WriteTree( DVS &dvs_, const std::string &dir_ )
{
  for ( auto const &entry : std::filesystem::directory_iterator( dir_ ) )
  {
    if ( IsIgnored( entry.path( ) ) )
    {
      continue;
    }

    if ( entry.is_regular_file( ) )
    {
      HashCommand hashCommand;

      auto [ err, hash ] = hashCommand.Hash( dvs_, entry.path( ).string( ) );

      if ( !err.empty( ) )
      {
        return err;
      }

      std::cout << hash << " " << entry.path( ) << std::endl;
    }
    else if ( entry.is_directory( ) )
    {
      if ( std::string err = WriteTree( dvs_, entry.path( ).string( ) );
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
