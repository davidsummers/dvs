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

  std::stringstream ss;
  ss << "tree " << writeTreeResult.oid << std::endl;
  ss << std::endl;
  ss << message_ << std::endl;

  HashCommand hashCommand;

  OidResult commitHashResult = hashCommand.Hash( dvs_, ss, ss.str( ).size( ), HashCommand::HashType::commit );

  if ( !commitHashResult.err.empty( ) )
  {
    result.err = commitHashResult.err;
  }
  else
  {
    result.oid = commitHashResult.oid;

    dvs_.SetHead( result.oid );
  }

  return result;
}
