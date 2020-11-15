//
// Tree Record
//

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "command_cat.h"
#include "command_hash.h"
#include "record_tree.h"

void TreeRecord::AddEntry( const std::string filename_, const RecordType &type_, std::string &hash_ )
{
  DirEntry entry;
  entry.filename         = filename_;
  entry.type             = type_;
  entry.oid              = hash_;
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

Error TreeRecord::Read( DVS &dvs_, const Oid &oid_ )
{
  Error             err;
  CatCommand        catCommand;
  std::stringstream treeSs;

  CatCommand::CatResult catResult = catCommand.GetHash( dvs_, oid_, &treeSs, RecordType::tree );

  if ( !catResult.err.empty( ) )
  {
    err = catResult.err;
    return err;
  }

  err = Parse( treeSs );

  return err;
}

Error TreeRecord::Parse( std::istream &s_ )
{
  // Get the tree hash and the parent hash (if any).
  while ( true )
  {
    std::string type;
    Oid         hash;
    std::string filename;
    std::string line;

    std::getline( s_, line );

    size_t pos = line.find( ' ' );
    if ( pos == std::string::npos )
    {
      break;
    }

    type = line.substr( 0, pos );
    hash = line.substr( pos + 1 );

    pos = hash.find( ' ' );

    if ( pos == std::string::npos )
    {
      break;
    }

    filename = hash.substr( pos + 1 );
    hash     = hash.substr( 0, pos );

    if ( type.empty( ) || hash.empty( ) || filename.empty( ) )
    {
      break;
    }

    RecordType recType = HashCommand::LookupType( type );

    if ( recType == RecordType::none )
    {
      std::stringstream ss;
      ss << "Error: RecordType == none while looking up record type.";
      return ss.str( );
    }

    AddEntry( filename, recType, hash );
  }

  return "";
}

std::ostream &operator<<( std::ostream &s_, const TreeRecord &treeRecord_ )
{
  treeRecord_.operator<<( s_ );
  return s_;
}
