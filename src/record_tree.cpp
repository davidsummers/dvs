//
// Tree Record
//

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "command_hash.h"
#include "record_tree.h"

void TreeRecord::AddEntry( const std::string filename_, const RecordType &type_, std::string &hash_ )
{
  DirEntry entry;
  entry.filename = filename_;
  entry.type = type_;
  entry.oid = hash_;
  m_DirList[ filename_ ] = entry;
}

std::ostream &TreeRecord::operator<<( std::ostream &s_ ) const
{
  for ( auto &entry : m_DirList )
  {
    s_ << HashCommand::LookupType( entry.second.type ) << " " << entry.second.oid << " " << entry.second.filename
       << std::endl;
  }

  return s_;
}

std::string TreeRecord::Parse( std::istream &s_ )
{
  std::string treeHash;
  std::string parentHash;
  std::string msg;

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
    std::getline( s_, input );

    if ( input.empty( ) )
    {
      break;
    }

    msg += input;
  }

  return "";
}

std::ostream &operator<<( std::ostream &s_, const TreeRecord &treeRecord_ )
{
  treeRecord_.operator<<( s_ );
  return s_;
}
