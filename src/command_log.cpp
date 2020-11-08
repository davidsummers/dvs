#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "command_cat.h"
#include "command_log.h"
#include "dvs.h"
#include "record_commit.h"

std::string LogCommand::ParseArgs( std::map< std::string, docopt::value > &args_ )
{
  std::string err;

  if ( docopt::value hashOption = args_[ "<hash>" ];
       hashOption && hashOption.isString( ) && !hashOption.asString( ).empty( ) )
  {
    m_HashId = hashOption.asString( );
  }

  return err;
}

std::string LogCommand::operator( )( DVS &dvs_ )
{
  if ( std::string validateError = dvs_.Validate( ); !validateError.empty( ) )
  {
    return validateError;
  }

  std::string result = GetLog( dvs_, m_HashId );

  return result;
}

std::string LogCommand::GetLog( DVS &dvs_, const std::string &hashId_ )
{
  std::string result;

  RefValue refValue{ false, hashId_ };

  if ( refValue.value.empty( ) )
  {
    refValue = dvs_.GetRef( s_HEAD_REF );
  }

  refValue.value = dvs_.GetOid( refValue.value );

  while ( !refValue.value.empty( ) )
  {
    CatCommand catCommand;
    std::stringstream commitSs;

    CatCommand::CatResult catResult = catCommand.GetHash( dvs_, refValue.value, &commitSs );

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

    CommitRecord commitRecord;

    result = commitRecord.Parse( commitSs );

    if ( result.empty( ) )
    {
      return result;
    }

    std::cout << "commit " << refValue.value << std::endl;
    std::cout << std::endl;
    std::cout << commitRecord.GetMsg( ) << std::endl;
    std::cout << std::endl;

    refValue.value = commitRecord.GetParentOid( );
  }

  // The rest of the contents of the commit is the commit message;
  return result;
}
