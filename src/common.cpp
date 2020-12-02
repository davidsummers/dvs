#include "common.h"

DirEntry::~DirEntry( )
{
  if ( m_Tree != nullptr )
  {
    delete m_Tree;
  }
}
