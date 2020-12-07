#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "command_cat.h"
#include "record_commit.h"

//
// Commit Record
//

Oid CommitRecord::GetTreeOid( ) const
{
  return m_TreeOid;
}

void CommitRecord::SetTreeOid( const Oid &oid_ )
{
  m_TreeOid = oid_;
}

const std::vector< Oid > &CommitRecord::GetParentOids( ) const
{
  return m_ParentOids;
}

void CommitRecord::AddParentOid( const Oid &oid_ )
{
  for ( const auto &entry : m_ParentOids )
  {
    if ( entry == oid_ )
    {
      return;
    }
  }

  m_ParentOids.push_back( oid_ );
}

std::string CommitRecord::GetMsg( ) const
{
  return m_Msg;
}

void CommitRecord::SetMsg( const std::string &msg_ )
{
  m_Msg = msg_;
}

Error CommitRecord::Read( DVS &dvs_, const Oid &oid_ )
{
  Error             err;
  CatCommand        catCommand;
  std::stringstream commitSs;

  CatCommand::CatResult catResult = catCommand.GetHash( dvs_, oid_, &commitSs, RecordType::commit );

  if ( !catResult.err.empty( ) )
  {
    err = catResult.err;
    return err;
  }

  err = Parse( commitSs );

  return err;
}

Error CommitRecord::Parse( std::istream &s_ )
{
  std::string type;
  Oid         oid;

  // Get the tree hash and the parent hash (if any).
  while ( true )
  {
    std::string line;

    std::getline( s_, line );

    size_t pos = line.find( ' ' );
    if ( pos == std::string::npos )
    {
      break;
    }

    type = line.substr( 0, pos );
    oid  = line.substr( pos + 1 );

    if ( type.empty( ) && oid.empty( ) )
    {
      break;
    }

    if ( type == "tree" )
    {
      m_TreeOid = oid;
    }
    else if ( type == "parent" )
    {
      m_ParentOids.push_back( oid );
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
    std::getline( s_, input );

    if ( input.empty( ) )
    {
      break;
    }

    m_Msg += input;
  }

  return "";
}

std::ostream &operator<<( std::ostream &s_, const CommitRecord &commitRecord_ )
{
  // Write out tree hash of this commit.
  s_ << "tree " << commitRecord_.GetTreeOid( ) << std::endl;

  // If we have a parent hash, write it out.
  for ( const auto &entryOid : commitRecord_.GetParentOids( ) )
  {
    s_ << "parent " << entryOid << std::endl;
  }

  // Write out blank line to separate headers from message body content.
  s_ << std::endl;
  // Write out commit message.
  s_ << commitRecord_.GetMsg( ) << std::endl;

  return s_;
}
