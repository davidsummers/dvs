#pragma once

//
// FileSystemBase
//

#include <filesystem>


class BaseFileSystem
{
  public:

    BaseFileSystem( );
    virtual ~BaseFileSystem( );

    virtual int mkdir( const std::filesystem::path & ) = 0;
    virtual int rmdir( const std::filesystem::path & ) = 0;

  protected:

  private:
};
