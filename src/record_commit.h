#pragma once

//
// Commit Record
//

#include <string>
#include <vector>

#include "common.h"

class CommitRecord
{
  public:
  Oid  GetTreeOid( ) const;
  void SetTreeOid( const Oid & );

  const std::vector< Oid > &GetParentOids( ) const;
  void                      AddParentOid( const Oid & );

  std::string GetMsg( ) const;
  void        SetMsg( const std::string & );

  Error Read( DVS &, const Oid & );

  protected:
  private:
  //
  // Data
  //

  Oid                m_TreeOid;
  std::vector< Oid > m_ParentOids;
  std::string        m_Msg;

  //
  // Methods
  //

  Error Parse( std::istream & );
};

std::ostream &operator<<( std::ostream &, const CommitRecord & );
