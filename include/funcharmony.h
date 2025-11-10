#ifndef FUNCHARMONIE_H
#define FUNCHARMONIE_H 1

#include <iostream>
#include <vector>
#include <array>
#include <functional>

#include "inja.hpp"
#include "textfile.h"

constexpr int aantal_chrom_noten = 12;
constexpr int aantal_diat_noten  = 7;

// ---------- ParserError ----------

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

// ---------- NootNaam ----------

class NootNaam
{
private:
   std::string naam;
   int         midi; // midi nummer binnen het niet-gestreepte octaaf

public:
   NootNaam(const std::string &nm, int mnr);
   ~NootNaam();
   std::string get_naam();
   int get_midi();
   void print();
};

// ---------- NootNamen ----------

class NootNamen
{
private:
   std::vector<NootNaam *> nootnamen;

public:
   NootNamen();
   ~NootNamen();
};

// ---------- Toets ----------

class Toets
{
public:
   virtual ~Toets() = 0;
   virtual void print() = 0;
   virtual bool heeft_naam(std::string nm) = 0;
   virtual NootNaam *get(int ri) = 0;            // geef NootNaam terug met juiste richting
   virtual NootNaam *get_nn(std::string nm) = 0; // geef NootNaam terug op naam
};

// ---------- Wit ----------

class Wit : public Toets
{
private:
   NootNaam *nootnaam;

public:
   Wit(NootNaam *nn);
   virtual ~Wit();
   virtual bool heeft_naam(std::string nm);
   virtual NootNaam *get(int ri);
   virtual NootNaam *get_nn(std::string nm);
   virtual void print();
};

// ---------- Zwart ----------

class Zwart : public Toets
{
private:
   NootNaam *nootnaam_laag;
   NootNaam *nootnaam_hoog;

public:
   Zwart(NootNaam *nn_lg, NootNaam *nn_hg);
   virtual ~Zwart();
   NootNaam *laag();
   NootNaam *hoog();

   virtual bool heeft_naam(std::string nm);
   virtual NootNaam *get(int ri);
   virtual NootNaam *get_nn(std::string nm);
   virtual void print();
};

// ---------- ToetsWijzer ----------

class Toetsen;

class ToetsWijzer
{
private:
   Toetsen *toetsen;
   int      index;

public:
   ToetsWijzer(Toetsen  *tt, int i);
   ~ToetsWijzer();
   NootNaam *get(int ri);
   void inc();
};

// ---------- Toetsen ----------

class Toetsen
{
private:
   std::vector<Toets *> toetsen;

public:
   Toetsen();
   ~Toetsen();
   Toets *get(int i);
   ToetsWijzer *zoek_noot_tw(std::string nm);
   NootNaam *zoek_noot(std::string nm);
   void print();
};

// ---------- StemBereik ----------

class Noot;

class StemBereik
{
private:
   Noot *laag;
   Noot *hoog;
   
public:
   StemBereik(Noot *lg, Noot *hg);
   ~StemBereik();
};

// ---------- StemBereiken ----------

class Harmonie;

class StemBereiken
{
private:
   std::array<StemBereik *, 4> bereiken;
   
public:
   StemBereiken();
   ~StemBereiken();
   void maak_bereiken(Harmonie *harmonie);
};

// ---------- AkkoordNoot ----------

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
   ~AkkoordNoot();
   void set_akkoord(Akkoord *akk);
   std::string get_naam();
   int get_stap() const;
};

// ---------- Akkoord ----------

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
   ~Akkoord();
   int get_basisnoot_rang() const;
   void print();
};

// ---------- Trap ----------


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
   ~Trap();
   std::string get_naam();
   int get_stap();
   NootNaam *get_noot();
   void add_akkoord(Akkoord *akk);
   void maak_akkoorden();
   void iterate(auto fu);
   void print();
};

// ---------- Toonladder ----------


class Toonladder
{
private:
   std::vector<Trap *> trappen;

public:
   Toonladder();
   ~Toonladder();
   Trap *get(int i);
   void add_trap(Trap *tr);
   void maak_akkoorden();
   static const bool lang = false;
   Trap *zoek_noot(std::string nt);
   Trap *zoek_functie(std::string fu);
   Trap *zoek_trap(NootNaam *nn);
   void print();
};

// ---------- Toonaard ----------

class Toonaard
{
private:
   std::string naam;
   int         richting; // 1 is kruis, -1 is bemol
   Toonladder *toonladder;

public:
   Toonaard(std::string nm, int ri);
   ~Toonaard();
   std::string get_naam();
   NootNaam *get(int k);
   int inc(int j);
   void maak_trappen(Toetsen *tt);
   Trap *zoek_noot(std::string nt);
   Trap *zoek_functie(std::string fu);
   Trap *zoek_trap(NootNaam *nn);
   void print();
};

// ---------- Toonaarden ----------


class Toonaarden
{
private:
   std::vector<Toonaard *> toonaarden;

public:
   Toonaarden();
   ~Toonaarden();
   void maak_trappen(Toetsen *tt);
   Toonaard *zoek(const std::string tna);
   Trap *zoek_trap(NootNaam *nn);
   void print();
};

// ---------- Harmonie ----------

/*
 * Harmonie bevat alle toetsen en toonaarden.
 */ 
class Harmonie
{
private:
   Toetsen      *toetsen;
   Toonaarden   *toonaarden;
   StemBereiken *bereiken;

public:
   Harmonie();   
   ~Harmonie();
   Toetsen *get_toetsen()
   {
      return toetsen;
   }
   Toonaard *zoek_toonaard(const std::string letters);
   Trap *zoek_trap(NootNaam *nn);
   void print();
};

// ---------- Fout ----------

class Fout
{
private:
   int nr;
   std::string tekst;   

public:
   Fout(int nnr, std::string txt);
   ~Fout();
   int get_nr();
   std::string get_tekst();
   void print();
};

// ---------- Noot ----------

class Noot
{
private:
   Trap       *trap; // weak ptr
   int         octaaf; // 1: ' 2: '' -1:,
   int         lengte; // 1, 2, 4 of 8
   std::string gelezen_tekst; // wordt gebruikt als trap null is
   int         midi;  // werkelijke toonhoogte

public:
   Noot(Trap *trp, int oct, int len, std::string tkst);
   ~Noot();
   //static Noot *maak_noot(NootNaam *nn, int oct);
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
   void print();
};

// ---------- Functie ----------

class Functie
{
private:
   Trap       *trap; // weak ptr
   std::string tekst;
   
public:
   Functie(Trap *trp, std::string tkst);   
   ~Functie();
   Trap       *get_trap();
   std::string get_tekst();
};

// ---------- Tel ----------

class Tel
{
private:
   static int            teller;
   int                   nr;
   int                   lengte;  // 1: heel, 2: half, 4: kwart, 8: achtste
   std::array<Noot *, 4> stemmen;
   Functie              *functie;
   Toonaard             *toonaard;
   std::vector<Fout *>   fouten;

public:
   Tel();
   ~Tel();
   Toonaard *get_toonaard();
   void  set_toonaard(Toonaard *tb);
   Noot *get_stem(int i);
   void  set_stem(int i, Noot *nt);
   void set_functie(Functie *fu);
   int get_nr()
   {
      return nr;
   }
   void print();
   nlohmann::json to_json();
};

// ---------- Maat ----------

class Maat
{
private:
   std::vector<Tel *> tellen;
   
public:
   Maat();
   ~Maat();
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
   nlohmann::json to_json();
   void for_each(std::function<void(Maat *, Tel *)> fu);   
};

// ---------- Lied ----------

class Lied
{
private:
   Harmonie           *harmonie;
   Textfile           *file;
   std::vector<Maat *> maten;
   std::vector<Fout *> fouten;
   
public:
   Lied(Harmonie *hrm, const std::string fn);
   ~Lied();
   unsigned long tellen_size();
   Tel *get_tel(unsigned long i);
   void nieuwe_maat();

   void parse_test_v1();
   void parse();
   void parse_linetype(const std::string ltype, bool maak_noot);
   void parse_v1(const std::string line, bool maak_noot);
   void parse_fu(const std::string line);
   void parse_ke(const std::string line);
   static int s_to_octaaf(std::string s);
   static int s_to_lengte(std::string s);
   void print();
   nlohmann::json to_json();
   void to_ly();
   void add_fout(Fout *f);
   void for_each(std::function<void(Maat *, Tel *)> fu);
   void maak_stemmen();
};

#endif
