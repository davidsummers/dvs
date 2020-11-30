#include <fstream>

#include "dvs.h"
#include "index.h"

void Index::AddEntry( const std::string &filename_, const Oid &oid_ )
{
  IndexEntry entry;
  entry.filename = filename_;
  entry.oid = oid_;
  m_IndexMap[ filename_ ] = entry;
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
    entry.filename = line.substr( 0, pos );
    entry.oid      = line.substr( pos + 1 );

    m_IndexMap[ entry.filename ] = entry; 
  }
  
  return "";
}

Error Index::Write( DVS &dvs_ )
{
  std::ofstream output( dvs_.GetSpecialPath( SpecialName::INDEX ), std::ios::binary );

  ForAllEntries( [ &output ] ( const IndexEntry &entry_ )
  {
    output << entry_.filename << " " << entry_.oid << std::endl;
  } );

  return "";
}
