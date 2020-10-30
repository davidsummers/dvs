#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "command_cat.h"
#include "command_read_tree.h"
#include "dvs.h"

std::string ReadTreeCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

  if ( docopt::value hashOption = args_[ "<hash>" ];
        hashOption && hashOption.isString( ) && !hashOption.asString( ).empty( ) )
  {
    m_HashId = hashOption.asString( );
  }
  else
  {
    err = "Missing hash ID.";
  }

  return err;
}


std::string ReadTreeCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }

  OidResult result = ReadTree( dvs_, m_HashId );

  return result.err;
}


OidResult ReadTreeCommand::ReadTree( DVS &dvs_, const std::string &hashId_ )
{
  EmptyCurrentDirectory( dvs_ );

  std::stringstream dirSs;
  {
    CatCommand::CatResult catResult;
    CatCommand catCommand;
    catResult = catCommand.GetHash( dvs_, hashId_, &dirSs );

    if ( !catResult.err.empty( ) )
    {
      OidResult result;
      result.err = catResult.err;
      return result;
    }
  }

  while ( dirSs )
  {
    std::string type;
    std::string hash;
    std::string filename;
    dirSs >> type >> hash >> filename;

    if ( type.empty( ) && hash.empty( ) && filename.empty( ) )
    {
      break;
    }

    if ( type == "blob" )
    {
       std::ofstream outFile( filename, std::ios_base::binary );
       CatCommand fileCat;
       fileCat.GetHash( dvs_, hash, &outFile );
    }
    else if ( type == "tree" )
    {
      std::filesystem::path curPath = std::filesystem::current_path( );
      std::filesystem::create_directory( filename );
      std::filesystem::current_path( filename );
      ReadTree( dvs_, hash );
      std::filesystem::current_path( curPath );
    }
    else
    {
      OidResult result;
      std::stringstream ss;
      ss << "Expected type 'blob' or 'tree' but got '" << type << "'." << std::endl;
      result.err = ss.str( );
      return result;
    }
  }

  OidResult result;
  return result;
}


void ReadTreeCommand::EmptyCurrentDirectory( DVS &dvs_ )
{
  for ( auto const &entry : std::filesystem::directory_iterator( "." ) )
  {
    if ( dvs_.IsIgnored( entry.path( ) ) )
    {
      continue;
    }

    if ( entry.is_regular_file( ) )
    {
      std::filesystem::remove( entry.path( ) );
    }
    else if ( entry.is_directory( ) )
    {
      std::filesystem::path saveDir = std::filesystem::current_path( );
      std::filesystem::current_path( entry.path( ).filename( ) );
      EmptyCurrentDirectory( dvs_ );
      std::filesystem::current_path( saveDir );
    }
  }
}
