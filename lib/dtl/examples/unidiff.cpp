#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <vector>

#include <dtl/dtl.hpp>
#include "common.hpp"

using namespace std;

using dtl::Diff;
using dtl::elemInfo;
using dtl::uniHunk;

static void showStats( const std::string &diffFilename1,
                       const std::string &actualFilename1,
                       const std::string &diffFilename2,
                       const std::string &actualFilename2 );
void unifiedDiff( string fp1, string fp2 ); 

static void showStats( const std::string &diffFilename1_,
                       const std::string &actualFilename1_,
                       const std::string &diffFilename2_,
                       const std::string &actualFilename2_ ) 
{
    const int    MAX_LENGTH    = 255;
    char         time_format[] = "%Y-%m-%d %H:%M:%S %z";
    time_t       rawtime[2];
    memset( &rawtime, 0, sizeof( rawtime ) );
    struct tm   *timeinfo[2];
    struct stat  st[2];
    memset( &st, 0, sizeof( st ) );
    
    if ( !actualFilename1_.empty( ) && stat( actualFilename1_.c_str( ), &st[0] ) == -1 )
    {
        cerr << "argv1 is invalid." << endl;
        exit(-1);
    }

    if ( !actualFilename2_.empty( ) && stat( actualFilename2_.c_str( ), &st[1] ) == -1 )
    {
        cerr << "argv2 is invalid" << endl;
        exit(-1);
    }
    
    char buf[2][MAX_LENGTH + 1];
    rawtime[0] = st[0].st_mtime;
    timeinfo[0] = localtime(&rawtime[0]);
    strftime(buf[0], MAX_LENGTH, time_format, timeinfo[0]);
    cout << "--- " << diffFilename1_ << '\t' << buf[0] << endl;
    rawtime[1] = st[1].st_mtime;
    timeinfo[1] = localtime(&rawtime[1]);
    strftime(buf[1], MAX_LENGTH, time_format, timeinfo[1]);
    cout << "+++ " << diffFilename2_ << '\t' << buf[1] << endl;
}

void unifiedDiff( string fp1_, string fp2_ )
{
  void unifiedDiff( const std::string &diffFilename,
                    const std::string &actualFilename1,
                    std::ifstream &str1,
                    const std::string &diffFilename2,
                    const std::string &actualFilename2,
                    std::ifstream &str2 );
  std::ifstream aifs( fp1_ );
  std::ifstream bifs( fp2_ );
  unifiedDiff( fp1_, fp1_, aifs, fp2_, fp2_, bifs );
}


void unifiedDiff( const std::string &diffFilename1_,
                  const std::string &actualFilename1_,
                  std::ifstream &str1_,
                  const std::string &diffFilename2_,
                  const std::string &actualFilename2_,
                  std::ifstream &str2_ )
{
    typedef string                 elem;
    typedef vector< elem >         sequence;
    typedef pair< elem, elemInfo > sesElem;

    elem          buf;
    sequence      ALines, BLines;
    
    while( getline( str1_, buf ) )
    {
        ALines.push_back( buf );
    }

    while( getline( str2_, buf ) )
    {
        BLines.push_back( buf );
    }
    
    Diff< elem > diff(ALines, BLines);
    diff.onHuge();
    //diff.onUnserious();
    diff.compose();
    
    // type unihunk definition test
    uniHunk< sesElem > hunk;
    
    if ( diff.getEditDistance( ) > 0 )
    {
        showStats( diffFilename1_, actualFilename1_, diffFilename2_, actualFilename2_ );             // show file info
     }
    
    diff.composeUnifiedHunks();
    diff.printUnifiedFormat();
}

#ifdef DTL_MAIN

int main(int argc, char *argv[])
{
    if (isFewArgs(argc)) {
        cerr << "Too few arguments." << endl;
        return -1;
    }
    
    string s1(argv[1]);
    string s2(argv[2]);
    bool   fileExist = true;
    
    if (!isFileExist(s1)) {
        cerr << s1 << " is invalid." << endl;
        fileExist = false;
    }
    
    if (!isFileExist(s2)) {
        cerr << s2 << " is invalid." << endl;
        fileExist = false;
    }
    
    if (!fileExist) {
        return -1;
    }
    
    unifiedDiff(s1, s2);
    return 0;
}

#endif
