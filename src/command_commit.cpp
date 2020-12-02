#include <iostream>
#include <sstream>

#include "command_commit.h"
#include "command_hash.h"
#include "command_write_tree.h"
#include "dvs.h"
#include "record_commit.h"

Error CommitCommand::ParseArgs( DocOptArgs &args_ )
{
  Error err;

  bool msgFlag = false;

  if ( docopt::value path = args_[ "<path>" ];
       path && path.isString( ) && !path.asString( ).empty( ) )
  {
    m_Path = path.asString( );
  }

  if ( docopt::value msgOption = args_[ "-m" ]; msgOption && msgOption.isBool( ) && msgOption.asBool( ) )
  {
    msgFlag = true;
  }
  else if ( docopt::value msgOption = args_[ "--message" ]; msgOption && msgOption.isBool( ) && msgOption.asBool( ) )
  {
    msgFlag = true;
  }

  if ( !msgFlag )
  {
    std::stringstream ss;
    ss << "Requires flag -m or --message.";
    return ss.str( );
  }

  if ( docopt::value msgValue = args_[ "<message>" ];
       msgValue && msgValue.isString( ) && !msgValue.asString( ).empty( ) )
  {
    m_Msg = msgValue.asString( );
  }
  else
  {
    std::stringstream ss;
    ss << "Missing hash identifier.";
    err = ss.str( );
  }

  return err;
}

Error CommitCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  OidResult result = Commit( dvs_, m_Msg );

  std::cout << "Commit Oid: " << result.oid << std::endl;

  return result.err;
}

OidResult CommitCommand::Commit( DVS &dvs_, const std::string &message_, const std::string &path_ )
{
  OidResult    result;
  CommitRecord commitRecord;

  commitRecord.SetMsg( message_ );

  WriteTreeCommand writeTreeCommand;

  std::string path = path_.empty( ) ? dvs_.GetTopLevelDirectory( ).string( ) : path_;

  OidResult writeTreeResult = writeTreeCommand.WriteTreeFromIndex( dvs_, path );

  if ( !writeTreeResult.err.empty( ) )
  {
    result.err = writeTreeResult.err;
    return result;
  }

  commitRecord.SetTreeOid( writeTreeResult.oid );

  RefValue parentRef = dvs_.GetRef( dvs_.GetSpecialName( SpecialName::HEAD ) );

  commitRecord.SetParentOid( parentRef.value );

  std::stringstream ss;

  ss << commitRecord;

  HashCommand hashCommand;

  OidResult commitHashResult = hashCommand.Hash( dvs_, ss, ss.str( ).size( ), RecordType::commit );

  if ( !commitHashResult.err.empty( ) )
  {
    result.err = commitHashResult.err;
  }
  else
  {
    result.oid = commitHashResult.oid;

    dvs_.SetRef( dvs_.GetSpecialName( SpecialName::HEAD ), RefValue{ false, result.oid } );
  }

  return result;
}
