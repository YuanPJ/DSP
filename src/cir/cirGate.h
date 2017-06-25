/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;
extern unsigned GlobalMark;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate(unsigned id = 0, unsigned l = 0, unsigned c = 0, unsigned m = 0):
        _id(id), _line(l), _col(c) , _mark(m){}
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const = 0; //{ return ""; }
   unsigned getLineNo() const { return _line; }
   unsigned getID() const { return _id; }
   unsigned getMark() const { return _mark; }
   string getSymbol() const { return _symbol; }

   // Printing functions
   virtual void printGate() const = 0;
   virtual void reportGate() const = 0;
   virtual void reportFanin(int level) = 0;
   virtual void reportFanout(int level) = 0;

   virtual void setOutGate(CirGate* gate, bool inv) {}
   virtual void resetGate(unsigned line, unsigned col, CirGate* in1, bool inv1, CirGate* in2, bool inv2) {}
   void setLineCol(unsigned line, unsigned col) { _line = line; _col = col; }
   void setSymbol(string symbol) { _symbol = symbol; }
   void setMark() { _mark = GlobalMark; }

   virtual bool checkUndef() const { return false; }
   virtual bool checkFloat() const { return false; }
   virtual bool checkUnused() const { return false; }
   virtual void dfs() {}
   virtual unsigned linkValue(bool which = true) const { return 0; }
   
private:
protected:
   unsigned _id;
   unsigned _line;
   unsigned _col;
   unsigned _mark;
   string _symbol;
};

class PiGate : public CirGate {
public:
    PiGate(unsigned id, unsigned l, unsigned c) : CirGate(id, l, c) {}
    ~PiGate() { _outList.resize(0); _invList.resize(0); }

    string getTypeStr() const { return "PI"; }
    void printGate() const;
    void reportGate() const;
    void reportFanin(int level);
    void reportFanout(int level);
    bool checkUnused() { if(_outList.empty()) return true; return false;}

    void setOutGate(CirGate* gate, bool inv) { _outList.push_back(gate); _invList.push_back(inv); }
    void dfs();
private:
    vector<CirGate*> _outList;
    vector<bool> _invList;
};

class PoGate : public CirGate {
public:
    PoGate(unsigned id, unsigned l, unsigned c, CirGate* in, bool inv)
           : CirGate(id, l, c), _in(in), _inv(inv) {}
    
    string getTypeStr() const { return "PO"; }
    void printGate() const; 
    void reportGate() const;
    void reportFanin(int level);
    void reportFanout(int level);
    bool checkFloat() const { if(_in->checkUndef()) return true; return false;}
    unsigned linkValue(bool which) const { if (true) return 2*(_in->getID()) + (_inv ? 1 : 0); }

    void dfs();
private:
    CirGate* _in;
    bool _inv;
};

class AigGate : public CirGate {
public:
    AigGate(unsigned id, unsigned l, unsigned c,
            CirGate* in1 = 0, bool inv1 = false, CirGate* in2 = 0, bool inv2 = false)
            : CirGate(id, l, c), _in1(in1), _inv1(inv1), _in2(in2), _inv2(inv2) {}
    ~AigGate() { _outList.resize(0); _invList.resize(0); }

    string getTypeStr() const { return "AIG"; }
    void printGate() const;
    void reportGate() const;
    void reportFanin(int level);
    void reportFanout(int level);
    
    void setOutGate(CirGate* gate, bool inv) { _outList.push_back(gate); _invList.push_back(inv); }
    void resetGate(unsigned line, unsigned col, CirGate* in1, bool inv1, CirGate* in2, bool inv2) {
        _in1 = in1; _inv1 = inv1;
        _in2 = in2; _inv2 = inv2;
        setLineCol(line, col);
    }
    bool checkUndef()  const{ if (_in1 == 0 && _in2 == 0) return true; return false; }
    bool checkFloat()  const{ if (_in1->checkUndef() || _in2->checkUndef()) return true; return false; }
    bool checkUnused() const{ if (_outList.empty()) return true; return false; }
    void dfs();
    unsigned linkValue(bool which) const { if (which) return 2*(_in1->getID()) + (_inv1 ? 1 : 0); 
                                           else       return 2*(_in2->getID()) + (_inv2 ? 1 : 0); }

private:
    CirGate* _in1;
    bool _inv1;
    CirGate* _in2;
    bool _inv2;
    vector<CirGate*> _outList;
    vector<bool> _invList;
};

class Const : public CirGate {
public:
    Const() : CirGate(0, 0, 0) {}
    ~Const() { _outList.resize(0); _invList.resize(0); }
    
    string getTypeStr() const { return "CONST"; }
    void printGate() const;
    void reportGate() const;
    void reportFanin(int level);
    void reportFanout(int level);
    bool checkUnused() const{ if(_outList.empty()) return true; return false;}

    void setOutGate(CirGate* gate, bool inv) { _outList.push_back(gate); _invList.push_back(inv); }
    void dfs();
private:
    vector<CirGate*> _outList;
    vector<bool> _invList;
};
#endif // CIR_GATE_H
