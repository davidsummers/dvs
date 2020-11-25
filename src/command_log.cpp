#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_cat.h"
#include "command_log.h"
#include "diff.h"
#include "dvs.h"
#include "record_commit.h"
#include "record_tree.h"

Error LogCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  Error err;

  if ( docopt::value hashOption = args_[ "<hash>" ];
       hashOption && hashOption.isString( ) && !hashOption.asString( ).empty( ) )
  {
    m_HashId = hashOption.asString( );
  }

  if ( docopt::value patchOption = args_[ "-p" ];
       patchOption && patchOption.isBool( ) && patchOption.asBool( ) )
  {
    m_ShowPatchDiff = true;
  }

  return err;
}

Error LogCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = GetLog( dvs_, m_HashId, m_ShowPatchDiff );

  return err;
}

Error LogCommand::GetLog( DVS &dvs_, const std::string &hashId_, const bool showPatchDiff_ )
{
  using RefMap = std::map< Oid, std::string >;
  RefMap refs;

  dvs_.ForAllRefs( "", true, [ &refs ] ( const std::string &refname_, const RefValue &refVal_ )
  {
    RefMap::iterator itr = refs.find( refVal_.value );

    if ( itr == refs.end( ) )
    {
      refs[ refVal_.value ] = refname_;
    }
    else
    {
      itr->second.append( ", " + refname_ );
    }
  } );

  Error err;

  RefValue refValue{ false, hashId_ };

  if ( refValue.value.empty( ) )
  {
    refValue = dvs_.GetRef( s_HEAD_REF );
  }

  refValue.value = dvs_.GetOid( refValue.value );

  while ( !refValue.value.empty( ) )
  {
    CommitRecord commitRecord;

    err = commitRecord.Read( dvs_, refValue.value );

    if ( !err.empty( ) )
    {
      return err;
    }

    RefMap::iterator itr = refs.find( refValue.value );

    std::cout << "commit " << refValue.value << ( itr == refs.end( ) ? "" : "( " + itr->second + " )" ) << std::endl;
    std::cout << std::endl;
    std::cout << commitRecord.GetMsg( ) << std::endl;
    std::cout << std::endl;

    if ( showPatchDiff_ )
    {
      Oid parentOid = commitRecord.GetParentOid( );

      if ( !parentOid.empty( ) )
      {
        CommitRecord parentCommitRecord;

        err = parentCommitRecord.Read( dvs_, parentOid );

        if ( !err.empty( ) )
        {
          return err;
        }

        TreeRecord commitTree;
        TreeRecord parentTree;

        err = commitTree.Read( dvs_, commitRecord.GetTreeOid( ) );

        if ( !err.empty( ) )
        {
          return err;
        }

        err = parentTree.Read( dvs_, parentCommitRecord.GetTreeOid( ) );

        if ( !err.empty( ) )
        {
          return err;
        }

        Diff::DiffTrees( dvs_, parentTree, commitTree );
      }
    }

    refValue.value = commitRecord.GetParentOid( );
  }

  // The rest of the contents of the commit is the commit message;
  return err;
}
