#pragma once

//
// Commit Record
//

#include <string>


class CommitRecord
{
  public:

    std::string GetTreeOid( ) const;
    void SetTreeOid( const std::string & );

    std::string GetParentOid( ) const;
    void SetParentOid( const std::string & );

    std::string GetMsg( ) const;
    void SetMsg( const std::string & );

    std::string Parse( std::istream & );

  protected:

  private:

    std::string m_TreeOid;
    std::string m_ParentOid;
    std::string m_Msg;
};


std::ostream &operator <<( std::ostream &, const CommitRecord & );
