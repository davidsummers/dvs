#pragma once

//
// Commit Record
//

#include <string>

#include "common.h"

class CommitRecord
{
  public:
  Oid  GetTreeOid( ) const;
  void SetTreeOid( const Oid & );

  Oid  GetParentOid( ) const;
  void SetParentOid( const Oid & );

  std::string GetMsg( ) const;
  void        SetMsg( const std::string & );

  Error Read( DVS &, const Oid & );

  protected:
  private:
  //
  // Data
  //

  Oid m_TreeOid;
  Oid m_ParentOid;
  std::string m_Msg;

  //
  // Methods
  //

  Error Parse( std::istream & );
};

std::ostream &operator<<( std::ostream &, const CommitRecord & );
