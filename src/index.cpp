#include <fstream>

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

  IndexEntry entry;
  entry.filename = pathName;
  entry.oid = oid;
  m_IndexMap[ pathName ] = entry;

  return "";
}

void Index::ForAllEntries( std::function< void ( const IndexEntry & ) > func_ )
{
  for ( const auto &entry : m_IndexMap )
  {
    func_( entry.second );
  }
}


Error Index::Read( DVS &dvs_ )
{
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

    IndexEntry entry;
    entry.oid      = line.substr( 0, pos );
    entry.filename = line.substr( pos + 1 );

    m_IndexMap[ entry.filename ] = entry; 
  }
  
  return "";
}

Error Index::Write( DVS &dvs_ )
{
  std::ofstream output( dvs_.GetSpecialPath( SpecialName::INDEX ), std::ios::binary );

  ForAllEntries( [ &output ] ( const IndexEntry &entry_ )
  {
    output << entry_.oid << " " << entry_.filename << std::endl;
  } );

  return "";
}
