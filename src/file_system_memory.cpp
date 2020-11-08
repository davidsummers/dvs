#include "file_system_memory.h"

MemoryFileSystem::MemoryFileSystem( )
{
}

MemoryFileSystem::~MemoryFileSystem( )
{
}

int MemoryFileSystem::mkdir( const std::filesystem::path & )
{
  return -1;
}

int MemoryFileSystem::rmdir( const std::filesystem::path & )
{
  return -1;
}
