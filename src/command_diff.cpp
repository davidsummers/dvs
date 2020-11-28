#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_diff.h"
#include "command_hash.h"
#include "command_write_tree.h"
#include "dvs.h"
#include "record_commit.h"

Error DiffCommand::ParseArgs( DocOptArgs & /* args_ */ )
{
  Error err;

  return err;
}

Error DiffCommand::operator( )( DVS &dvs_ )
{
  if ( Error validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  Error err = Diff( dvs_ );

  return err;
}

Error DiffCommand::Diff( DVS &dvs_ )
{
  Error err;

  // CommitRecord     commitRecord;
  WriteTreeCommand writeTreeCommand;

  OidResult writeTreeResult = writeTreeCommand.WriteTree( dvs_, "." );

  if ( !writeTreeResult.err.empty( ) )
  {
    err = writeTreeResult.err;
    return err;
  }

  commitRecord.SetTreeOid( writeTreeResult.oid );

  RefValue parentRef = dvs_.GetRef( s_HEAD_REF );

  commitRecord.SetParentOid( parentRef.value );

  std::stringstream ss;

  ss << commitRecord;

  HashCommand hashCommand;

  OidResult commitHashResult = hashCommand.Hash( dvs_, ss, ss.str( ).size( ), RecordType::commit );

  if ( !commitHashResult.err.empty( ) )
  {
    err = commitHashResult.err;
  }
  else
  {
    Oid oid = commitHashResult.oid;

    dvs_.SetRef( s_HEAD_REF, RefValue{ false, oid } );
  }

  return err;
}
