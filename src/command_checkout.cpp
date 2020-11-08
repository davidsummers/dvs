#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_cat.h"
#include "command_checkout.h"
#include "command_read_tree.h"
#include "dvs.h"

std::string CheckoutCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

  if ( docopt::value branchNameOption = args_[ "<BranchName>" ];
       branchNameOption && branchNameOption.isString( ) && !branchNameOption.asString( ).empty( ) )
  {
    m_BranchName = branchNameOption.asString( );
  }

  return err;
}

std::string CheckoutCommand::operator( )( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  std::string result = Checkout( dvs_, m_BranchName );

  return result;
}

std::string CheckoutCommand::Checkout( DVS &dvs_, const std::string &branchName_ )
{
  std::string result;

  std::string oid = dvs_.GetOid( branchName_ );

  RefValue refValue{ false, oid };

  if ( refValue.value.empty( ) )
  {
    refValue = dvs_.GetRef( s_HEAD_REF );
  }

  if ( refValue.value.empty( ) )
  {
    return "Expected non-empty Hash ID.";
  }

  refValue.value = dvs_.GetOid( refValue.value );

  CatCommand catCommand;
  std::stringstream commitSs;

  CatCommand::CatResult catResult = catCommand.GetHash( dvs_, refValue.value, &commitSs, RecordType::commit );

  if ( !catResult.err.empty( ) )
  {
    result = catResult.err;
    return result;
  }

  if ( catResult.type != "commit" )
  {
    std::stringstream ss;
    ss << "Dvs log command expected 'commit' type for Hash ID '" << refValue.value << "'" << std::endl;
    return ss.str( );
  }

  std::string treeHash;
  std::string parentHash;
  std::string msg;

  std::string type;
  std::string hash;

  // Get the tree hash and the parent hash (if any).
  while ( true )
  {
    std::string line;

    std::getline( commitSs, line );

    size_t pos = line.find( ' ' );
    if ( pos == std::string::npos )
    {
      break;
    }

    type = line.substr( 0, pos );
    hash = line.substr( pos + 1 );

    if ( type.empty( ) && hash.empty( ) )
    {
      break;
    }

    if ( type == "tree" )
    {
      treeHash = hash;
    }
    else if ( type == "parent" )
    {
      parentHash = hash;
    }
    else
    {
      std::stringstream ss;
      ss << "Log command: Unknown type '" << type << "'." << std::endl;
      return ss.str( );
    }
  }

  while ( true )
  {
    std::string input;
    std::getline( commitSs, input );

    if ( input.empty( ) )
    {
      break;
    }

    msg += input;
  }

  // Now read the resulting tree
  ReadTreeCommand readTreeCommand;
  OidResult readTreeResult = readTreeCommand.ReadTree( dvs_, treeHash );

  if ( !readTreeResult.err.empty( ) )
  {
    result = readTreeResult.err;
    return result;
  }

  if ( IsBranch( dvs_, branchName_ ) )
  {
    std::string branchName = s_REFS_BRANCHES_LOCAL;
    branchName += branchName_;
    refValue = RefValue{ true, branchName };
  }
  else
  {
    refValue = RefValue{ false, oid };
  }

  // Now save the commit hash in .dvs/HEAD
  const bool deref = false;
  dvs_.SetRef( s_HEAD_REF, refValue, deref );

  return result;
}

bool CheckoutCommand::IsBranch( DVS &dvs_, const std::string &branchName_ )
{
  RefValue refValue = dvs_.GetRef( s_REFS_BRANCHES_LOCAL + branchName_ );
  bool isBranch = !refValue.value.empty( );
  return isBranch;
}
