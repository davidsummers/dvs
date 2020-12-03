#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_cat.h"
#include "command_read_tree.h"
#include "command_branch_switch.h"
#include "dvs.h"

Error SwitchBranchCommand::ParseArgs( DocOptArgs &args_ )
{
  Error err;

  if ( docopt::value branchNameOption = args_[ "<BranchName>" ];
       branchNameOption && branchNameOption.isString( ) && !branchNameOption.asString( ).empty( ) )
  {
    m_BranchName = branchNameOption.asString( );
  }
  else
  {
    err = "Missing required branch name.";
  }

  return err;
}

Error SwitchBranchCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = Switch( dvs_, m_BranchName );

  return err;
}

Error SwitchBranchCommand::Switch( DVS &dvs_, const std::string &branchName_ )
{
  Error err;

  Oid oid = dvs_.GetOid( branchName_ );

  RefValue refValue{ false, oid };

  if ( refValue.value.empty( ) )
  {
    refValue = dvs_.GetRef( dvs_.GetSpecialName( SpecialName::HEAD ) );
  }

  if ( refValue.value.empty( ) )
  {
    return "Expected non-empty Hash ID.";
  }

  refValue.value = dvs_.GetOid( refValue.value );

  CatCommand        catCommand;
  std::stringstream commitSs;

  CatCommand::CatResult catResult = catCommand.GetHash( dvs_, refValue.value, &commitSs, RecordType::commit );

  if ( !catResult.err.empty( ) )
  {
    err = catResult.err;
    return err;
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
  OidResult       readTreeResult = readTreeCommand.ReadTreeToDirectory( dvs_, treeHash );

  if ( !readTreeResult.err.empty( ) )
  {
    err = readTreeResult.err;
    return err;
  }

  if ( IsBranch( dvs_, branchName_ ) )
  {
    std::string branchName = dvs_.GetSpecialName( SpecialName::BRANCHES_LOCAL );
    branchName += branchName_;
    refValue = RefValue{ true, branchName };
  }
  else
  {
    refValue = RefValue{ false, oid };
  }

  // Now save the commit hash in .dvs/HEAD
  const bool deref = false;
  dvs_.SetRef( dvs_.GetSpecialName( SpecialName::HEAD ), refValue, deref );

  return err;
}

bool SwitchBranchCommand::IsBranch( DVS &dvs_, const std::string &branchName_ )
{
  RefValue refValue = dvs_.GetRef( dvs_.GetSpecialName( SpecialName::BRANCHES_LOCAL ) + branchName_ );
  bool     isBranch = !refValue.value.empty( );
  return isBranch;
}
