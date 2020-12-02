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

DirEntry &TreeRecord::AddEntry( const std::string filename_, const RecordType &type_, const std::string &hash_ )
{
  DirEntry entry;
  entry.filename         = filename_;
  entry.type             = type_;
  entry.oid              = hash_;
  m_DirList[ filename_ ] = entry;

  return m_DirList[ filename_ ];
}

std::ostream &TreeRecord::operator<<( std::ostream &s_ ) const
{
  ForAllEntries( [ &s_ ]( const DirEntry &entry_ )
  {
    s_ << HashCommand::LookupType( entry_.type ) << " " << entry_.oid << " " << entry_.filename << std::endl;
  } );

  return s_;
}

void TreeRecord::ForAllEntries( std::function< void( DirEntry & ) > func_ )
{
  for ( DirList::iterator itr = m_DirList.begin( );
        itr != m_DirList.end( );
        ++itr )
  {
    DirEntry &dirEntry = itr->second;
    func_( dirEntry );
  }
}

void TreeRecord::ForAllEntries( std::function< void( const DirEntry & ) > func_ ) const
{
  for ( DirList::const_iterator itr = m_DirList.begin( );
        itr != m_DirList.end( );
        ++itr )
  {
    const DirEntry &dirEntry = itr->second;
    func_( dirEntry );
  }
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

OidResult TreeRecord::Write( DVS &dvs_ )
{
  OidResult   result;
  HashCommand hashCommand;

  std::stringstream ss;

  ss << *this;

  auto [ hashErr, oid ] = hashCommand.Hash( dvs_, ss, ss.str( ).size( ), RecordType::tree );

  // std::cout << "Directory End: " << oid << std::endl;

  result.err = hashErr;
  result.oid = oid;

  return result;
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
