#include <iostream>
#include <sstream>

#include "command_hash.h"
#include "command_write_tree.h"
#include "dvs.h"
#include "record_tree.h"


std::string WriteTreeCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }

  OidResult result = WriteTree( dvs_ );

  std::cout << "Top Level Directory: " << result.oid << std::endl;

  return result.err;
}


OidResult WriteTreeCommand::WriteTree( DVS &dvs_, const std::string &dir_ )
{
  OidResult result;

  TreeRecord treeRecord;

  for ( auto const &entry : std::filesystem::directory_iterator( dir_ ) )
  {
    if ( dvs_.IsIgnored( entry.path( ) ) )
    {
      continue;
    }

    if ( entry.is_regular_file( ) )
    {
      HashCommand hashCommand;

      auto [ err, hash ] = hashCommand.Hash( dvs_, entry.path( ).string( ), RecordType::blob  );

      if ( !err.empty( ) )
      {
        result.err = err;
        return result;
      }

      treeRecord.AddEntry( entry.path( ).filename( ).string( ), RecordType::blob, hash );
    }
    else if ( entry.is_directory( ) )
    {
      OidResult writeResult = WriteTree( dvs_, entry.path( ).string( ) );
      if ( !writeResult.err.empty( ) )
      {
        result.err = writeResult.err; 
        return result;
      }

      treeRecord.AddEntry( entry.path( ).filename( ).string( ), RecordType::tree, writeResult.oid );
    }
    else
    {
      std::stringstream ss;
      ss << "Unknown file type for " << entry.path( ) << ".";
      result.err = ss.str( );
      return result;
    }
  }

  HashCommand hashCommand;

  std::stringstream ss;

  ss << treeRecord;

  auto [ hashErr, oid ] = hashCommand.Hash( dvs_, ss, ss.str( ).size( ), RecordType::tree );

  // std::cout << "Directory End: " << oid << std::endl;
  
  result.err = hashErr;
  result.oid = oid;
  return result;
}
