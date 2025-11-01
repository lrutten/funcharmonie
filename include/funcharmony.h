#ifndef FUNCHARMONIE_H
#define FUNCHARMONIE_H 1

#include <iostream>
#include <vector>
#include <array>

#include "textfile.h"

constexpr int aantal_chrom_noten = 12;
constexpr int aantal_diat_noten  = 7;

class ParserError
{
private:
   std::string melding;

public:
   ParserError(std::string mld) : melding(mld)
   {
   }
   std::string get_melding() const
   {
      return melding;
   }
};

class NootNaam
{
private:
   std::string naam;
   int         midi; // midi nummer

public:
   NootNaam(const std::string &nm, int mnr);
   std::string get_naam();
   int get_midi();
   void print();
};

class NootNamen
{
private:
   std::vector<NootNaam *> nootnamen;

public:
   NootNamen();
};


class Toets
{
public:
   virtual void print() = 0;
   virtual bool heeft_naam(std::string nm) = 0;
   virtual NootNaam *get(int ri) = 0;
};

class Wit : public Toets
{
private:
   NootNaam *nootnaam;

public:
   Wit(NootNaam *nn);

   virtual bool heeft_naam(std::string nm);
   virtual NootNaam *get(int ri);
   virtual void print();
};

class Zwart : public Toets
{
private:
   NootNaam *nootnaam_laag;
   NootNaam *nootnaam_hoog;

public:
   Zwart(NootNaam *nn_lg, NootNaam *nn_hg);
   NootNaam *laag();
   NootNaam *hoog();

   virtual bool heeft_naam(std::string nm);
   virtual NootNaam *get(int ri);

   virtual void print();
};

class Toetsen;

class ToetsWijzer
{
private:
   Toetsen *toetsen;
   int      index;

public:
   ToetsWijzer(Toetsen  *tt, int i);
   NootNaam *get(int ri);
   void inc();
};


class Toetsen
{
private:
   std::vector<Toets *> toetsen;

public:
   Toetsen();
   Toets *get(int i);
   ToetsWijzer *zoek_noot(std::string nm);
   void print();
};

class Akkoord;

class AkkoordNoot
{
private:
   Akkoord  *akkoord;
   NootNaam *noot;
   int       rol;     // 0: grondnoot, 1: terts, 2: kwint
   int       stap;

public:
   AkkoordNoot(NootNaam *nt, int rl, int st);
   void set_akkoord(Akkoord *akk);
   std::string get_naam();
   int get_stap() const;
};

class Trap;

class Akkoord
{
private:
   Trap        *trap;
   int          omkering; // 0: basis, 1: sixtakkoord, 2: kwartsixtakkoord
   AkkoordNoot *g; // grondnoot
   AkkoordNoot *t; // terts
   AkkoordNoot *k; // kwint

public:
   Akkoord(Trap *tr, AkkoordNoot *gg, AkkoordNoot *tt, AkkoordNoot *kk, int omk);
   int get_basisnoot_rang() const;
   void print();
};


class Toonaard;

class Trap
{
private:
   std::string naam; // I, II, III, IV, V, VI, VII
   int         stap; // trap 0 - 6
   Toonaard   *toonaard;
   NootNaam   *noot;
   std::vector<Akkoord *> akkoorden;

public:
   Trap(std::string nm, int stp, Toonaard *tona, NootNaam *nt);
   std::string get_naam();
   int get_stap();
   NootNaam *get_noot();
   void add_akkoord(Akkoord *akk);
   void maak_akkoorden();
   void iterate(auto fu);
   void print();
};


class Toonladder
{
private:
   std::vector<Trap *> trappen;

public:
   Toonladder();
   Trap *get(int i);
   void add_trap(Trap *tr);
   void maak_akkoorden();
   static const bool lang = false;
   Trap *zoek_noot(std::string nt);
   Trap *zoek_functie(std::string fu);
   void print();
};


class Toonaard
{
private:
   std::string naam;
   int         richting; // 1 is kruis, -1 is bemol
   Toonladder *toonladder;

public:
   Toonaard(std::string nm, int ri);
   std::string get_naam();
   NootNaam *get(int k);
   int inc(int j);
   void maak_trappen(Toetsen *tt);
   Trap *zoek_noot(std::string nt);
   Trap *zoek_functie(std::string fu);
   void print();
};



class Toonaarden
{
private:
   std::vector<Toonaard *> toonaarden;

public:
   Toonaarden();
   void maak_trappen(Toetsen *tt);
   Toonaard *zoek(const std::string tna);
   void print();
};

// ---------- Harmonie ----------

/*
 * Harmonie bevat alle toetsen en toonaarden.
 */ 
class Harmonie
{
private:
   Toetsen    *toetsen;
   Toonaarden *toonaarden;

public:
   Harmonie();   
   ~Harmonie();
   Toonaard *zoek_toonaard(const std::string letters);
   void print();
};


class Noot
{
private:
   Trap *trap;
   int   octaaf; // 1: ' 2: '' -1:,
   int   lengte; // 1, 2, 4 of 8

public:
   Noot(Trap *trp, int oct, int len);
   Trap *get_trap()
   {
      return trap;
   }
   int get_octaaf()
   {
      return octaaf;
   }
   int get_lengte()
   {
      return lengte;
   }
   std::string to_s();
};

class Functie
{
private:

public:
   Functie();   
};

class Tel
{
private:
   int                   lengte;  // 1: heel, 2: half, 4: kwart, 8: achtste
   std::array<Noot *, 4> stemmen;
   Functie              *functie;
   Toonaard             *toonaard;

public:
   Tel();
   Toonaard *get_toonaard();
   void  set_toonaard(Toonaard *tb);
   Noot *get_stem(int i);
   void  set_stem(int i, Noot *nt);
   void print();
};

class Maat
{
private:
   std::vector<Tel *> tellen;
   
public:
   Maat();
   unsigned long size()
   {
      return tellen.size();
   }
   void add(Tel *tel)
   {
      tellen.push_back(tel);
   }
   Tel *get(unsigned long i)
   {
      return tellen[i];
   }
   void print();
};

class Lied
{
private:
   Harmonie           *harmonie;
   Textfile           *file;
   std::vector<Maat *> maten;
   
public:
   Lied(Harmonie *hrm, const std::string fn);
   unsigned long tellen_size();
   Tel *get_tel(unsigned long i);
   void nieuwe_maat();

   void parse_test_v1();
   void parse();
   void parse_linetype(const std::string ltype, bool maak_noot);
   void parse_v1(const std::string line, bool maak_noot);
   void parse_fu(const std::string line);
   void parse_ke(const std::string line);
   int s_to_octaaf(std::string s);
   int s_to_lengte(std::string s);
   void print();
   void to_ly_voice(std::ofstream &lyf, int i);
   void to_ly();
};

#endif
