#ifndef FUNCHARMONIE_H
#define FUNCHARMONIE_H 1

#include <iostream>
#include <vector>
#include <array>
#include <set>
#include <functional>

#include "inja.hpp"
#include "textfile.h"

constexpr int aantal_chrom_noten = 12;
constexpr int aantal_diat_noten  = 7;

constexpr int midi_laag = 21;   // laagste midicode A0
constexpr int midi_hoog = 127;  // hoogste midicode G8

extern std::map<std::string, std::set<std::string>>functies;

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
class Noot;
class Toets;

class NootNaam
{
private:
   std::string naam;
   int         midi; // midi nummer binnen het niet-gestreepte octaaf
   Toets      *toets;

public:
   NootNaam(const std::string &nm, int mnr);
   ~NootNaam();
   std::string get_naam();
   int get_midi();
   Toets *get_toets();
   void set_toets(Toets *t);
   bool equals_enharmonic(std::string nt);
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
   int       rol;   // 0: grondnoot, 1: terts, 2: kwint
   int       stap;  // de ligging 0-6 van deze noot in de toonaard 
                    // bv c: 0, e: 2, g: 4 
public:
   AkkoordNoot(NootNaam *nt, int rl, int st);
   ~AkkoordNoot();
   void set_akkoord(Akkoord *akk);
   std::string get_naam();
   int get_stap() const;
   NootNaam *get_noot()
   {
      return noot;
   }
};

// ---------- AkkoordWijzer ----------

class Akkoord;

class AkkoordWijzer
{
private:
   Akkoord *akkoord;
   int      index;

public:
   AkkoordWijzer(Akkoord  *ak, int i);
   ~AkkoordWijzer();
   NootNaam *get();
   int geti();
   void dec();
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
   int get_omkering()
   {
      return omkering;
   }
   Trap *get_trap()
   {
      return trap;
   }
   bool bevat(NootNaam *nn);
   int get_basisnoot_rang() const;
   NootNaam *get(int i);
   AkkoordWijzer *zoek(NootNaam *nn);
   Noot *maak_noot(NootNaam *nn, int len);
   void print();
};

// ---------- Trap ----------


class Toonaard;

class Trap
{
private:
   std::string naam;      // I   II    III  IV  V   VI    VII
                          //  IIb  IIIb       Vb VIb  VIIb
   int         stap;      // trap 0 - 11
   int         diat_step; // diatonische trap 0 - 6
   Toonaard   *toonaard;
   NootNaam   *noot;
   bool        diatonic;
   std::vector<Akkoord *> akkoorden;

public:
   Trap(std::string nm, int stp, Toonaard *tona, NootNaam *nt, bool diat = true);
   ~Trap();
   std::string get_naam();
   bool is_diatonic()
   {
      return diatonic;
   }
   int get_stap();
   int get_diatonic_step();
   void set_diatonic_step(int st);
   Toonaard *get_toonaard()
   {
      return toonaard;
   }
   Akkoord *get_basisakkoord()
   {
      return akkoorden[0];
   }
   NootNaam *get_noot();
   void add_akkoord(Akkoord *akk);
   void maak_akkoorden();
   Akkoord *zoek_akkoord(int omk);
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
   int diat_plus(int s1, int s2);
   void add_trap(Trap *tr);
   void set_diat_steps();
   void maak_akkoorden();
   static const bool lang = true;
   Trap *zoek_noot(std::string nt);
   Trap *zoek_functie(std::string fu);
   Trap *zoek_trap(NootNaam *nn);
   std::vector<Akkoord *> lijst_pasakkoorden(NootNaam *nn);
   void print();
};

// ---------- Toonaard ----------

class Toonaard
{
private:
   std::string naam;
   int         richting; // 1 is kruis, -1 is bemol
   bool        minor;    // is dit in mineur?
   Toonladder *toonladder;

public:
   Toonaard(std::string nm, int ri, bool mnr = false);
   ~Toonaard();
   std::string get_naam();
   NootNaam *get(int k);
   int inc_not_in_use(int j);
   int diat_plus(int s1, int s2);
   void maak_trappen(Toetsen *tt);
   Trap *zoek_noot(std::string nt);
   Trap *zoek_functie(std::string fu);
   Trap *zoek_trap(NootNaam *nn);
   std::vector<Akkoord *> lijst_pasakkoorden(NootNaam *nn);
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

// ---------- ANoot ----------

class ANoot
{
protected:
   int         lengte; // 1, 2, 4 of 8

public:
   ANoot(int len);
   virtual ~ANoot();
   int get_lengte()
   {
      return lengte;
   }
   virtual bool is_rust() = 0;
   virtual Trap *get_trap() = 0;
   virtual std::string to_s() = 0;
   virtual void print() = 0;
};

// ---------- Noot ----------

class Noot : public ANoot
{
private:
   Trap       *trap; // weak ptr
   int         octaaf; // 1: ' 2: '' -1:,
   std::string gelezen_tekst; // wordt gebruikt als trap null is
   int         midi;  // werkelijke toonhoogte

public:
   Noot(Trap *trp, int oct, int len, std::string tkst);
   ~Noot();
   bool is_rust();
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
   int get_midi()
   {
      return midi;
   }
   void set_midi(int mdi)
   {
      midi = mdi;
   }
   void onder(Noot *nt);
   bool operator<(Noot &);
   void verlaag();
   void verhoog();
   std::string to_s();
   void print();
};

// ---------- Rust ----------

class Rust : public ANoot
{
private:

public:
   Rust(int len);
   ~Rust();
   bool is_rust();
   Trap *get_trap();
   std::string to_s();
   void print();
};


// ---------- Functie ----------
enum Ligging
{
   lig_geen  = 0, 
   lig_eng   = 1,
   lig_wijd  = 2,
   lig_xwijd = 3
};

const std::string_view ew_to_s(Ligging ew);

class Functie
{
private:
   Trap       *trap; // weak ptr
   std::string tekst;
   std::string kwartsixt;
   Ligging     engwijd;
   std::string plusmin;
   
public:
   Functie(Trap *trp, std::string tkst, std::string kwsxt, std::string ew, std::string plmn);   
   ~Functie();
   Trap       *get_trap();
   std::string get_tekst();
   std::string get_kwartsixt()
   {
      return kwartsixt;
   }
   Ligging get_engwijd()
   {
      return engwijd;
   }
   std::string get_plusmin()
   {
      return plusmin;
   }
};

// ---------- Tel ----------

class Tel
{
private:
   static int            teller;
   int                   nr;
   int                   lengte;  // 1: heel, 2: half, 4: kwart, 8: achtste
   std::array<ANoot *, 4> stemmen;
   Functie              *functie;
   Toonaard             *toonaard;
   std::vector<Fout *>   fouten;

public:
   Tel();
   ~Tel();
   Toonaard *get_toonaard();
   void  set_toonaard(Toonaard *tb);
   ANoot *get_stem(int i);
   void  set_stem(int i, ANoot *nt);
   Functie *get_functie()
   {
      return functie;
   }
   void set_functie(Functie *fu);
   int get_nr()
   {
      return nr;
   }
   int get_lengte()
   {
      return lengte;
   }
   void set_lengte(int l)
   {
      lengte = l;
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
   Harmonie             *harmonie;
   Textfile             *file;
   std::vector<Maat *>   maten;
   std::map<int, Fout *> fouten;
   std::string           titel;
   std::string           metrum;
   std::string           tempo;
   std::string           toonaard;
   
public:
   Lied(Harmonie *hrm, const std::string fn);
   ~Lied();
   unsigned long tellen_size();
   Tel *get_tel(unsigned long i);
   void nieuwe_maat();

   void parse_test_v1();
   void parse();
   void parse_linetype(const std::string ltype, bool maak_noot);
   void parse_header();
   void parse_v1(const std::string line, bool maak_noot);
   void parse_fu(const std::string line);
   void parse_ke(const std::string line);
   static int s_to_octaaf(std::string s);
   static int s_to_lengte(std::string s);
   void print();
   nlohmann::json to_json();
   void to_ly(std::string fn);
   void add_fout(Fout *f);
   void for_each(std::function<void(Maat *, Tel *)> fu);
   void vul_rusten(Tel *t);
   void zet_stemmen_in_tel(Tel *t, Functie *fu, Akkoord *akk, Noot *n_sop, NootNaam *nn_alt, NootNaam *nn_ten, NootNaam *nn_bas);
   void maak_stemmen();
};

#endif
