#include <iostream>
#include <sstream>

#include "command_commit.h"
#include "command_hash.h"
#include "command_write_tree.h"
#include "dvs.h"


std::string CommitCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

  bool msgFlag = false;

  if ( docopt::value msgOption = args_[ "-m" ];
       msgOption && msgOption.isBool( ) && msgOption.asBool( ) )
  {
    msgFlag = true;
  }
  else if ( docopt::value msgOption = args_[ "--message" ];
       msgOption && msgOption.isBool( ) && msgOption.asBool( ) )
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


std::string CommitCommand::operator ( ) ( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( );
       !validateError.empty( ) )
  {
    return validateError;
  }

  OidResult result = Commit( dvs_, m_Msg );

  std::cout << "Commit Oid: " << result.oid << std::endl;

  return result.err;
}


OidResult CommitCommand::Commit( DVS &dvs_, const std::string &message_ )
{
  OidResult result;

  WriteTreeCommand writeTreeCommand;

  OidResult writeTreeResult = writeTreeCommand.WriteTree( dvs_, "." );

  if ( !writeTreeResult.err.empty( ) )
  {
    result.err = writeTreeResult.err;
    return result;
  }

  RefValue parentRef = dvs_.GetRef( s_HEAD_REF );

  std::stringstream ss;

  // Write out tree hash of this commit.
  ss << "tree " << writeTreeResult.oid << std::endl;

  // If we have a parent hash, write it out.
  if ( !parentRef.value.empty( ) )
  {
    ss << "parent " << parentRef.value << std::endl;
  }

  // Write out blank line to separate headers from message body content.
  ss << std::endl;
  // Write out commit message.
  ss << message_ << std::endl;

  HashCommand hashCommand;

  OidResult commitHashResult = hashCommand.Hash( dvs_, ss, ss.str( ).size( ), RecordType::commit );

  if ( !commitHashResult.err.empty( ) )
  {
    result.err = commitHashResult.err;
  }
  else
  {
    result.oid = commitHashResult.oid;

    dvs_.SetRef( s_HEAD_REF, RefValue{ false, result.oid } );
  }

  return result;
}
