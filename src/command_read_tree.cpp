#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "command_cat.h"
#include "command_hash.h"
#include "command_read_tree.h"
#include "dvs.h"
#include "index.h"
#include "record_tree.h"

Error ReadTreeCommand::ParseArgs( DocOptArgs &args_ )
{
  Error err;

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

Error ReadTreeCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  OidResult result = ReadTreeToDirectory( dvs_, m_HashId );

  return result.err;
}

OidResult ReadTreeCommand::ReadTreeToDirectory( DVS &dvs_, const std::string &hashId_ )
{
  EmptyCurrentDirectory( dvs_ );

  std::string hashId = hashId_;
  hashId             = dvs_.GetOid( hashId );

  OidResult result;

  TreeRecord tree;

  result.err = tree.Read( dvs_, hashId );

  if ( !result.err.empty( ) )
  {
    return result;
  }

  tree.ForAllEntries( [ &dvs_, &result, this ] ( const DirEntry &entry_ )
  {
    if ( entry_.type == RecordType::blob )
    {
      std::ofstream outFile( entry_.filename, std::ios_base::binary );
      CatCommand    fileCat;
      fileCat.GetHash( dvs_, entry_.oid, &outFile, RecordType::blob );
    }
    else if ( entry_.type == RecordType::tree )
    {
      std::filesystem::path curPath = std::filesystem::current_path( );
      std::filesystem::create_directory( entry_.filename );
      std::filesystem::current_path( entry_.filename );
      ReadTreeToDirectory( dvs_, entry_.oid );
      std::filesystem::current_path( curPath );
    }
    else
    {
      OidResult         result;
      std::stringstream ss;
      ss << "Expected type 'blob' or 'tree' but got '" << HashCommand::LookupType( entry_.type ) << "'." << std::endl;
      result.err = ss.str( );
    }
  } );

  return result;
}


OidResult ReadTreeCommand::ReadTreeToIndex( DVS &dvs_, Index &index_, const std::string &hashId_, const std::string &pathPrefix_ )
{
  OidResult result;

  std::string hashId = hashId_;
  hashId             = dvs_.GetOid( hashId );

  TreeRecord tree;

  result.err = tree.Read( dvs_, hashId ); 

  if ( !result.err.empty( ) )
  {
    return result;
  }

  tree.ForAllEntries( [ &dvs_, &index_, &pathPrefix_, &result, this ] ( const DirEntry &entry_ )
  {
    if ( entry_.type == RecordType::blob )
    {
      DirEntry newEntry = entry_;
      newEntry.filename = pathPrefix_ + entry_.filename;
      index_.AddEntry( dvs_, newEntry );
    }
    else if ( entry_.type == RecordType::tree )
    {
      result = ReadTreeToIndex( dvs_, index_, entry_.oid, entry_.filename + "/" );
    }
  } );

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
