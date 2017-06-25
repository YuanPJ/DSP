/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
unsigned GlobalMark = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirMgr::~CirMgr(void)
{
    for (unsigned i = 0; i<_totalList.size(); i++) {
        if (_totalList[i] != 0) {
            delete _totalList[i];
        }
    }
    _totalList.clear();
    _piList.resize(0);
    _poList.resize(0);
    _netList.resize(0);
    _floatList.resize(0);
    _unusedList.resize(0);
}

bool
CirMgr::readMatrix(const string& fileName)
{
    fstream matFile;
    matFile.open(fileName.c_str());
    if (matFile.fail()) {
        cout << "Cannot open file \"" << fileName << "\"!!" << endl;
        return false;
    }

    int users = 0, movies = 0, ratings = 0;
    while (matFile.good()) {
        string line;
        int userId, movieId;
        getline(matFile, line, ',');
        myStr2Int(line, userId);
        getline(matFile, line, ',');
        myStr2Int(line, movieId);
        getline(matFile, line);
        users = userId > users ? userId : users;
        movies = movieId > movies ? movieId : movies;
    }
    double** mat = new double*[users+1];
    for(int i = 0; i < users+1; ++i) {
        mat[i] = new double[movies+1];
        for (int j = 0; j < movies+1; ++j)
            mat[i][j] = 0.0;
    }
    matFile.close();

    matFile.open(fileName.c_str());
    while (matFile.good()) {
        string line;
        int userId, movieId;
        double rating;
        getline(matFile, line, ',');
        myStr2Int(line, userId);
        getline(matFile, line, ',');
        myStr2Int(line, movieId);
        getline(matFile, line, ',');
        rating = atof(line.c_str());
        ++ratings;
        getline(matFile, line);
        mat[userId][movieId] = rating;
    }
    matFile.close();

    vector<double> count;
    count.resize(movies+1);
    int userCount = 0, movieCount = 0;
    for (int i = 0; i < users+1; ++i) {
        bool exist = false;
        for (int j = 0; j < movies+1; ++j) {
            if (mat[i][j] != 0) {
                count[j] += mat[i][j];
                exist = true;
            }
        }
        if (exist) userCount++;
    }
    for (int j = 0; j < movies+1; ++j) {
        if (count[j] > 0) movieCount++;
    }

    _ratingMatrix = mat;
    _maxUserId = users;
    _maxMovieId = movies;
    _ratings = ratings;
    _users = userCount;
    _movies = movieCount;
    return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
    cout << endl;
    cout << "Matrix Statistics" << endl
         << "==================" << endl
         << "      USERS " << setw(11) << right << _users << endl
         << "     MOVIES " << setw(11) << right << _movies << endl
         << "    RATINGS " << setw(11) << right << _ratings << endl
         << "------------------" << endl
         << " MAX_USERID " << setw(11) << right << _maxUserId << endl
         << "MAX_MOVIEID " << setw(11) << right << _maxMovieId << endl;
}

void
CirMgr::printSettings() const
{
    
}

void
CirMgr::train()
{
    _latent = 200;
    _iterations = 1000;
    _learningRate = 0.01;
    _lambda = 0.0;
    double** userMatrix = new double*[_maxUserId+1];
    for (int i = 0; i < _maxUserId+1; ++i) {
        userMatrix[i] = new double[_latent];
        for (int j = 0; j < _latent; ++j) {
            userMatrix[i][j] = (rand() % 2000 - 1000) * 0.0001;
        }
    }
    double** movieMatrix = new double*[_latent];
    for (int i = 0; i < _latent; ++i) {
        movieMatrix[i] = new double[_maxMovieId+1];
        for (int j = 0; j < _maxMovieId+1; ++j) {
            movieMatrix[i][j] = (rand() % 2000 - 1000) * 0.0001;
        }
    }

    for (int iters = 1; iters < _iterations; ++iters) {
        for (int i = 0; i < _maxUserId+1; ++i) {
            for (int j = 0; j < _maxMovieId+1; ++j) {
                double sum = 0.0;
                for (int k = 0; k < _latent; ++k) 
                    sum += userMatrix[i][k] * movieMatrix[k][j];
                if (_ratingMatrix[i][j] > 0) {
                    double eij = _ratingMatrix[i][j] - sum;
                    for (int k = 0; k < _latent; ++k) {
                        userMatrix[i][k] += _learningRate * (eij * movieMatrix[k][j] - _lambda * userMatrix[i][k]);
                        movieMatrix[k][j] += _learningRate * (eij * userMatrix[i][k] - _lambda * movieMatrix[k][j]);
                    }
                }
            }
        }
        double e = 0;
        for (int i = 0; i < _maxUserId+1; ++i) {
            for (int j = 0; j < _maxMovieId+1; ++j) {
                double sum = 0.0;
                for (int k = 0; k < _latent; ++k)
                    sum += userMatrix[i][k] * movieMatrix[k][j];
                if (_ratingMatrix[i][j] > 0) {
                    e += (_ratingMatrix[i][j] - sum) * (_ratingMatrix[i][j] - sum);
                    for (int k = 0; k < _latent; ++k) 
                        e += _lambda * ( userMatrix[i][k] * userMatrix[i][k] + 
                                        movieMatrix[k][j] * movieMatrix[k][j] );
                }
            }
        }
        cout << "iterations: " << iters << ", traning error: " << e << endl; 
    }


    _userMatrix = userMatrix;
    _movieMatrix = movieMatrix;
}

void
CirMgr::printPIs() const
{
    unsigned size = _piList.size();
    cout << "PIs of the circuit:";
    for (unsigned i = 0; i < size; i++) cout << ' ' << _piList[i]->getID();
    cout << endl;
}

void
CirMgr::printPOs() const
{
    unsigned size = _poList.size();
    cout << "POs of the circuit:";
    for (unsigned i = 0; i < size; i++) cout << ' ' << _poList[i]->getID();
    cout << endl;
}

void
CirMgr::printFloatGates() const
{
    unsigned sizef = _floatList.size(), sizeu = _unusedList.size();
    if (sizef) {
        cout << "Gates with floating fanin(s):";
        for (unsigned i = 0; i < sizef; i++) cout << ' ' << _floatList[i];
        cout << endl;
    }
    if (sizeu) {
        cout << "Gates defined but not used  :";
        for (unsigned i = 0; i < sizeu; i++) cout << ' ' << _unusedList[i];
        cout << endl;
    }
}

void
CirMgr::writeAag(ostream& outfile) const
{
    unsigned aigs = 0;
    vector<CirGate*> aigList;
    for (unsigned i = 0; i < _netList.size();i++){
        if(_netList[i]->getTypeStr() == "AIG") {
            ++aigs; aigList.push_back(_netList[i]);
        }
    }
    outfile << "aag " << _max << ' ' << _pis << " 0 " << _pos << ' ' << aigs << endl;
    for (unsigned i = 0; i < _piList.size(); i++) outfile << 2*(_piList[i]->getID()) << endl;
    for (unsigned i = 0; i < _poList.size(); i++) outfile << _poList[i]->linkValue() << endl;
    for (unsigned i = 0; i < aigs; i++) {
            outfile << 2*(aigList[i]->getID()) << ' ' 
                    << aigList[i]->linkValue() << ' ' << aigList[i]->linkValue(false) << endl;
    }
    for (unsigned i = 0; i < _piList.size(); i++){
        string sym = _piList[i]->getSymbol();
        if (!sym.empty()) outfile << 'i' << i << ' ' << sym << endl;
    }
    for (unsigned i = 0; i < _poList.size(); i++){
        string sym = _poList[i]->getSymbol();
        if (!sym.empty()) outfile << 'o' << i << ' ' << sym << endl;
    }
}

void
CirMgr::traversal()
{
    GlobalMark++;
    for (unsigned i = 0; i < _pos; i++) {
        _poList[i]->dfs();
        setNet(_poList[i]);
    }
    unsigned size = _totalList.size();
    for (unsigned i = 1; i < size; i++) {
        if(_totalList[i] == 0 || _totalList[i]->checkUndef()) continue;
        if(_totalList[i]->checkFloat())   _floatList.push_back(_totalList[i]->getID());
        if(_totalList[i]->checkUnused()) _unusedList.push_back(_totalList[i]->getID());
    }
}
