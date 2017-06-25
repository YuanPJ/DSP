/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
    CirMgr() {}
    ~CirMgr();
    // Access functions
    // return '0' if "gid" corresponds to an undefined gate.
    CirGate* getGate(unsigned gid) const { return _totalList[gid]; }

    void setPiGate(CirGate* pi, unsigned id) { _piList.push_back(pi); setGate(pi, id); }
    void setPoGate(CirGate* po, unsigned id) { _poList.push_back(po); setGate(po, id); }
    void setGate(CirGate* gate, unsigned id) { _totalList[id] = gate; }
    void setNet(CirGate* gate) { _netList.push_back(gate); }

    // Member functions about circuit construction
    bool readMatrix(const string&);

    // Member functions about circuit reporting
    void printSummary() const;
    void printSettings() const;

    void train();

    void printPIs() const;
    void printPOs() const;
    void printFloatGates() const;
    void writeAag(ostream&) const;
    void traversal();

private:
    double** _ratingMatrix;
    double** _userMatrix;
    double** _movieMatrix;
    int _maxUserId, _maxMovieId, _users, _movies, _ratings;
    int _latent, _iterations;
    double _learningRate, _lambda;

    vector<CirGate*> _piList;
    vector<CirGate*> _poList;
    vector<CirGate*> _totalList;
    vector<CirGate*> _netList;
    vector<unsigned> _floatList;
    vector<unsigned> _unusedList;
    unsigned _max;
    unsigned _pis;
    unsigned _pos;
    unsigned _aigs;
};

#endif // CIR_MGR_H
