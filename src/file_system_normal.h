#pragma once

//
// FileSystemBase
//

#include "file_system_base.h"

class NormalFileSystem : public BaseFileSystem
{
  public:
  NormalFileSystem( );
  virtual ~NormalFileSystem( );

  virtual int mkdir( const std::filesystem::path & ) override;
  virtual int rmdir( const std::filesystem::path & ) override;

  protected:
  private:
};
