/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;
extern unsigned GlobalMark;

static int GlobalLevel = -1;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
PiGate::printGate() const {
    cout << "PI  " << getID();
    if (!_symbol.empty()) cout << " (" << _symbol << ')';
    cout << endl;
}

void
PiGate::reportGate() const{
    stringstream ss;
    ss << _id << ' ' << _line+1;
    string id, line;
    ss >> id >> line;
    cout << "==================================================" << endl;
    if (_symbol.empty()) {
        string print = "= PI(" + id + "), line " + line;
        cout << setw(48) << left << print << " =" << endl;
    }
    else {
        string print = "= PI(" + id + ")\"" + _symbol + "\", line " + line;
        cout << setw(48) << left << print << " =" << endl;
    }
    cout << "==================================================" << endl;
}

void
PiGate::reportFanin(int level) {
    assert (level >= 0);
    cout << "PI " << getID() << endl;
}

void
PiGate::reportFanout(int level) {
    assert (level >= 0);
    if (GlobalLevel < 0) { GlobalLevel = level; GlobalMark++; }
    cout << "PI " << getID() << endl;
    if (level != 0) {
        for (unsigned i = 0; i < _outList.size(); i++) {
            cout << "  ";
            if (_invList[i]) cout << '!';
            _outList[i]->reportFanout(level-1);
        }
    }
    if (GlobalLevel == level) GlobalLevel = -1;
}

void
PiGate::dfs() {}

void
PoGate::printGate() const {
    cout << "PO  " << getID() << ' ';
    if (_in->checkUndef()) cout << '*';
    if (_inv) cout << '!';
    cout << _in->getID();
    if (!_symbol.empty()) cout << " (" << _symbol << ')';
    cout << endl;
}

void
PoGate::reportGate() const{
    stringstream ss;
    ss << _id << ' ' << _line+1;
    string id, line;
    ss >> id >> line;
    cout << "==================================================" << endl;
    if (_symbol.empty()) {
        string print = "= PO(" + id + "), line " + line;
        cout << setw(48) << left << print << " =" << endl;
    }
    else {
        string print = "= PO(" + id + ")\"" + _symbol + "\", line " + line;
        cout << setw(48) << left << print << " =" << endl;
    }
    cout << "==================================================" << endl;
}

void
PoGate::reportFanin(int level) {
    assert (level >= 0);
    if (GlobalLevel < 0) { GlobalLevel = level; GlobalMark++; }
    cout << "PO " << getID() << endl;
    if (level != 0) {
        cout << "  ";
        if (_inv) cout << '!';
        _in->reportFanin(level-1);
    }
    if (GlobalLevel == level) GlobalLevel = -1;
}

void
PoGate::reportFanout(int level) {
    assert (level >= 0);
    cout << "PO " << getID() << endl;
}

void
PoGate::dfs(){
    if ((_in->getMark() == GlobalMark)) return;
    _in->setMark();
    if (!(_in->checkUndef())) { _in->dfs(); cirMgr->setNet(_in);}
}

void
AigGate::printGate() const {
    cout << "AIG " << getID() << ' ';
    if (_in1->checkUndef()) cout << '*';
    if (_inv1) cout << '!';
    cout << _in1->getID() << ' ';

    if (_in2->checkUndef()) cout << '*';
    if (_inv2) cout << '!';
    cout << _in2->getID();
    if (!_symbol.empty()) cout << " (" << _symbol << ')';
    cout << endl;
}

void
AigGate::reportGate() const{
    stringstream ss;
    ss << _id << ' ' << _line+1;
    string id, line;
    ss >> id >> line;
    cout << "==================================================" << endl;
    if (checkUndef()) {
        string print = "= UNDEF(" + id + "), line 0";
        cout << setw(48) << left << print << " =" << endl;
    }
    else {
        string print = "= AIG(" + id + "), line " + line;
        cout << setw(48) << left << print << " =" << endl;
    }
    cout << "==================================================" << endl;
}

void
AigGate::reportFanin(int level) {
    assert (level >= 0);
    if (GlobalLevel < 0) { GlobalLevel = level; GlobalMark++; }
    if (checkUndef()) cout << "UNDEF " << getID() << endl;
    else {
        cout << "AIG " << getID();
        if (level != 0) {
            if (_mark == GlobalMark) cout << " (*)" << endl;
            else {
                cout << endl;
                setMark();
                for(int j = 0; j <= GlobalLevel-level; j++) cout << "  ";
                if (_inv1) cout << '!';
                _in1->reportFanin(level-1);

                for(int j = 0; j <= GlobalLevel-level; j++) cout << "  ";
                if (_inv2) cout << '!';
                _in2->reportFanin(level-1);
            }
        }
        else cout << endl;
    }
    if (GlobalLevel == level) GlobalLevel = -1;
}

void
AigGate::reportFanout(int level) {
    assert (level >= 0);
    if (GlobalLevel < 0) { GlobalLevel = level; GlobalMark++; }
    cout << "AIG " << getID();
    if (level != 0) {
        if (_mark == GlobalMark) cout << " (*)" << endl;
        else if (!_outList.empty()) {
            cout << endl;
            setMark();
            for (unsigned i = 0; i < _outList.size(); i++) {
                for(int j = 0; j <= GlobalLevel-level; j++) cout << "  ";
                if (_invList[i]) cout << '!';
                _outList[i]->reportFanout(level-1);
            }
        }
        else cout << endl;
    }
    else cout << endl;
    if (GlobalLevel == level) GlobalLevel = -1;
}

void
AigGate::dfs(){
    if (!(_in1->getMark() == GlobalMark)) {
        _in1->setMark();
        if (!(_in1->checkUndef())) { _in1->dfs(); cirMgr->setNet(_in1); }
    }
    if (!(_in2->getMark() == GlobalMark)) {
        _in2->setMark();
        if (!(_in2->checkUndef())) { _in2->dfs(); cirMgr->setNet(_in2); }
    }
}

void
Const::printGate() const {
    cout << "CONST0" << endl;
}

void
Const::reportGate() const{
    cout << "==================================================" << endl;
    cout << "= CONST(0), line 0                               =" << endl;
    cout << "==================================================" << endl;
}

void
Const::reportFanin(int level) {
    assert (level >= 0);
    cout << "CONST 0" << endl;
}

void
Const::reportFanout(int level) {
    assert (level >= 0);
    if (GlobalLevel < 0) { GlobalLevel = level; GlobalMark++; }
    cout << "CONST 0" << endl;
    if (level != 0) {
        for (unsigned i = 0; i < _outList.size(); i++) {
            cout << "  ";
            if (_invList[i]) cout << '!';
            _outList[i]->reportFanout(level-1);
        }
    }
    if (GlobalLevel == level) GlobalLevel = -1;
}

void
Const::dfs() {}

