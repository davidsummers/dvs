#include "common.h"
#include "record_tree.h"

DirEntry::~DirEntry( )
{
  if ( m_Tree != nullptr )
  {
    delete m_Tree;
  }
}
