#include <fstream>
#include <sstream>

#include "dvs.h"
#include "index.h"
#include "command_hash.h"

Error Index::AddEntry( DVS &dvs_, const std::string &pathName_ )
{
  std::string pathName = pathName_;

  for ( size_t pos = pathName.find( '\\' ); pos != std::string::npos; pos = pathName.find( '\\' ) )
  {
    pathName[ pos ] = '/';
  }

  if ( std::filesystem::is_directory( pathName ) )
  {
    for ( const auto &entry : std::filesystem::directory_iterator( pathName ) )
    {
      if ( Error err = AddEntry( dvs_, entry.path( ).string( ) ); !err.empty( ) )
      {
        return err;
      }
    }

    return "";
  }

  HashCommand hashCommand;

  auto [ err, oid ] = hashCommand.Hash( dvs_, pathName, RecordType::blob );

  if ( !err.empty( ) )
  {
    return err;
  }

  DirEntry entry;
  entry.type = RecordType::blob;
  entry.oid = oid;
  entry.filename = pathName;
  m_IndexMap[ pathName ] = entry;

  m_Dirty = true;

  return "";
}

void Index::AddEntry( DVS &dvs_, const DirEntry &entry_ )
{
  m_IndexMap[ entry_.filename ] = entry_;
  m_Dirty = true;
}

Error Index::RemoveEntry( DVS &dvs_, const std::string &pathName_ )
{
  std::string pathName = pathName_;

  for ( size_t pos = pathName.find( '\\' ); pos != std::string::npos; pos = pathName.find( '\\' ) )
  {
    pathName[ pos ] = '/';
  }

  IndexMap::iterator itr = m_IndexMap.find( pathName );

  if ( itr != m_IndexMap.end( ) )
  {
    m_IndexMap.erase( itr );
  }
  else
  {
    std::stringstream ss;
    ss << "File '" << pathName << "' not found in index." << std::endl;
  }

  m_Dirty = true;
  
  return "";
}

void Index::ForAllEntries( std::function< void ( const DirEntry & ) > func_ )
{
  for ( const auto &entry : m_IndexMap )
  {
    func_( entry.second );
  }
}


Error Index::Read( DVS &dvs_ )
{
  if ( m_Read )
  {
    return "";
  }

  m_IndexMap.clear( );

  std::filesystem::path indexPath = dvs_.GetSpecialPath( SpecialName::INDEX );

  if ( !std::filesystem::exists( indexPath ) )
  {
    return "Index file does not exist.";
  }

  std::ifstream input( dvs_.GetSpecialPath( SpecialName::INDEX ), std::ios::binary );

  while ( input )
  {
    std::string line;
    std::getline( input, line );

    if ( line.empty( ) )
    {
      break;
    }

    size_t pos = line.find( ' ' );

    if ( pos == std::string::npos )
    {
      break; // Something is wrong, this is not the line you are looking for.
    }

    DirEntry entry;
    entry.type = HashCommand::LookupType( line.substr( 0, pos ) );
    entry.oid  = line.substr( pos + 1 );

    pos = entry.oid.find( ' ' );

    if ( pos == std::string::npos )
    {
      break; // Something is wrong, this is not the line you are looking for.
    }

    entry.filename = entry.oid.substr( pos + 1 );
    entry.oid = entry.oid.substr( 0, pos );
    m_IndexMap[ entry.filename ] = entry; 
  }

  m_Read = true; // We've now read this index so normally no need to re-read it.

  return "";
}

Error Index::Write( DVS &dvs_ )
{
  if ( !m_Dirty )
  {
    return ""; // Not dirty so no need to to re-write.
  }

  std::ofstream output( dvs_.GetSpecialPath( SpecialName::INDEX ), std::ios::binary );

  ForAllEntries( [ &output ] ( const DirEntry &entry_ )
  {
    output << HashCommand::LookupType( entry_.type ) << " " << entry_.oid << " " << entry_.filename << std::endl;
  } );

  m_Dirty = false; // No need to re-write it unless it becomes dirty again.

  return "";
}
