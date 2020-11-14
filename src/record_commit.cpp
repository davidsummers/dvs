#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "record_commit.h"

//
// Commit Record
//

std::string CommitRecord::GetTreeOid( ) const
{
  return m_TreeOid;
}

void CommitRecord::SetTreeOid( const std::string &oid_ )
{
  m_TreeOid = oid_;
}

std::string CommitRecord::GetParentOid( ) const
{
  return m_ParentOid;
}

void CommitRecord::SetParentOid( const std::string &oid_ )
{
  m_ParentOid = oid_;
}

std::string CommitRecord::GetMsg( ) const
{
  return m_Msg;
}

void CommitRecord::SetMsg( const std::string &msg_ )
{
  m_Msg = msg_;
}

std::string CommitRecord::Parse( std::istream &s_ )
{
  std::string type;
  std::string hash;

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
    hash = line.substr( pos + 1 );

    if ( type.empty( ) && hash.empty( ) )
    {
      break;
    }

    if ( type == "tree" )
    {
      m_TreeOid = hash;
    }
    else if ( type == "parent" )
    {
      m_ParentOid = hash;
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
  if ( !commitRecord_.GetParentOid( ).empty( ) )
  {
    s_ << "parent " << commitRecord_.GetParentOid( ) << std::endl;
  }

  // Write out blank line to separate headers from message body content.
  s_ << std::endl;
  // Write out commit message.
  s_ << commitRecord_.GetMsg( ) << std::endl;

  return s_;
}
