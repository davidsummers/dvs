//
// DVC declaration
//

class DVC
{
  public:

    DVC( );
    ~DVC( );

    // Sub-commands.
    void Init( );
    void Checkout( );
    void Commit( );
    void Fetch( );
    void Pull( );
    void Push( );
    void Status( );

  protected:

  private:
};
