#pragma once

//
// FileSystemBase
//

#include "file_system_base.h"

class MemoryFileSystem : public BaseFileSystem
{
  public:
  MemoryFileSystem( );
  virtual ~MemoryFileSystem( );

  virtual int mkdir( const std::filesystem::path & ) override;
  virtual int rmdir( const std::filesystem::path & ) override;

  protected:
  private:
};
