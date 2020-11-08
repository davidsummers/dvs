#include "file_system_normal.h"

NormalFileSystem::NormalFileSystem( )
{
}

NormalFileSystem::~NormalFileSystem( )
{
}

int NormalFileSystem::mkdir( const std::filesystem::path & )
{
  return -1;
}

int NormalFileSystem::rmdir( const std::filesystem::path & )
{
  return -1;
}
