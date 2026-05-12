
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <regex>
#include <cassert>

#include "inja.hpp"
#include "funcharmony.h"


static const bool debug = false;

// Dit zijn de toegelaten overgangen
std::map<std::string, std::set<std::string>>functies
{
   // van    naar
   { "I",   { "V", "IV", "VI", "III", "VII" } },
   { "II",  { "V", "VII" } },
   { "III", { "IV", "VI" } },
   { "IV",  { "V", "I" , "II", "VII"} },
   { "V",   { "I", "III", "VI" } },
   { "VI",  { "II", "III", "IV" , "VII"} },
   { "VII", { "I" } },
};

std::array<std::array<std::string, 2>, 4> stembereiken
{{
   { "c'", "a''"},  // sopraan
   { "g", "c''" },  // alt
   { "c", "g'" },   // tenor
   { "e,", "c'" }   // bas
}};



std::string to_lower(const std::string &s)
{
   std::string low = "";
   for (const auto &c : s)
   {
      auto c2 = tolower(c);
      low += c2;
   }

   return low;
}

// ---------- NootNaam ----------

NootNaam::NootNaam(const std::string &nm, int mnr) : naam(nm), midi(mnr)
{
}

NootNaam::~NootNaam()
{
   //std::cout << "~NootNaam\n";
}

std::string NootNaam::get_naam()
{
   return naam;
}

int NootNaam::get_midi()
{
   return midi;
}

void NootNaam::print()
{
   std::cout << naam << "\n";
}


// ---------- NootNamen ----------

NootNamen::NootNamen()
{
}

// ---------- Toets ----------

Toets::~Toets()
{
   //std::cout << "~Toets\n";
}


// ---------- Wit ----------

Wit::Wit(NootNaam *nn) : nootnaam(nn)
{
}

Wit::~Wit()
{
   //std::cout << "~Wit\n";
   delete nootnaam;
}

bool Wit::heeft_naam(std::string nm)
{
   return nm == nootnaam->get_naam();
}

NootNaam *Wit::get(int ri)
{
   return nootnaam;
}

NootNaam *Wit::get_nn(std::string nn)
{
   return nootnaam;
}

void Wit::print()
{
   std::cout << "Wit\n";
   nootnaam->print();
}


// ---------- Zwart ----------

Zwart::Zwart(NootNaam *nn_lg, NootNaam *nn_hg) : nootnaam_laag(nn_lg), nootnaam_hoog(nn_hg)
{
}

Zwart::~Zwart()
{
   //std::cout << "~Zwart\n";
   delete nootnaam_laag;
   delete nootnaam_hoog;
}

NootNaam *Zwart::laag()
{
   return nootnaam_laag;
}

NootNaam *Zwart::hoog()
{
   return nootnaam_hoog;
}

bool Zwart::heeft_naam(std::string nm)
{
   if (nm == nootnaam_laag->get_naam())
   {
      return true;
   }
   else
   if (nm == nootnaam_hoog->get_naam())
   {
      return true;
   }
   else
   {
      return false;
   }
}

NootNaam *Zwart::get(int ri)
{
   if (ri > 0)
   {
      return nootnaam_hoog;
   }
   else
   {
      return nootnaam_laag;
   }
}

NootNaam *Zwart::get_nn(std::string nm)
{
   if (nm == nootnaam_hoog->get_naam())
   {
      return nootnaam_hoog;
   }
   else
   {
      return nootnaam_laag;
   }
}

void Zwart::print()
{
   std::cout << "Zwart\n";
   nootnaam_laag->print();
   nootnaam_hoog->print();
}


// ---------- ToetsWijzer ----------

ToetsWijzer::ToetsWijzer(Toetsen  *tt, int i) : toetsen(tt), index(i)
{
}

ToetsWijzer::~ToetsWijzer()
{
   //std::cout << "~ToetsWijzer\n";
}

NootNaam *ToetsWijzer::get(int ri)
{
   Toets *t = toetsen->get(index);
   return t->get(ri);
}

void ToetsWijzer::inc()
{
   index++;
   if (index >= aantal_chrom_noten)
   {
      index = 0;
   }
   if (debug) std::cout << "         index " << index << "\n";
}


// ---------- Toetsen ----------

/*
 * De notennamen krijgen elk ook een midi nummer.
 * Dit start met 48, dit is het nummer van C3 of c in Lilypond.
 */ 
Toetsen::Toetsen()
{
   toetsen.push_back(new Wit(new NootNaam("c", 48)));
   toetsen.push_back(new Zwart(new NootNaam("des", 49), new NootNaam("cis", 49)));
   toetsen.push_back(new Wit(new NootNaam("d", 50)));
   toetsen.push_back(new Zwart(new NootNaam("es", 51), new NootNaam("dis", 51)));
   toetsen.push_back(new Wit(new NootNaam("e", 52)));
   toetsen.push_back(new Wit(new NootNaam("f", 53)));
   toetsen.push_back(new Zwart(new NootNaam("ges", 54), new NootNaam("fis", 54)));
   toetsen.push_back(new Wit(new NootNaam("g", 55)));
   toetsen.push_back(new Zwart(new NootNaam("as", 56), new NootNaam("gis", 56)));
   toetsen.push_back(new Wit(new NootNaam("a", 57)));
   toetsen.push_back(new Zwart(new NootNaam("bes", 58), new NootNaam("ais", 58)));
   toetsen.push_back(new Wit(new NootNaam("b", 59)));
}

Toetsen::~Toetsen()
{
   //std::cout << "~Toetsen\n";
   for (Toets *t: toetsen)
   {
      delete t;
   }
}

Toets *Toetsen::get(int i)
{
   return toetsen[i];
}

/**
 * Zoek een noot en geef een ToetsWijzer terug.
 */ 
ToetsWijzer *Toetsen::zoek_noot_tw(std::string nm)
{
   int i = 0;
   for (Toets *t: toetsen)
   {
      if (t->heeft_naam(nm))
      {
         if (debug) std::cout << "   gevonden\n";
         return new ToetsWijzer(this, i);
      }
      i++;
   }
   if (debug) std::cout << "   niet gevonden\n";
   return nullptr;
}


NootNaam *Toetsen::zoek_noot(std::string nm)
{
   for (Toets *t: toetsen)
   {
      if (t->heeft_naam(nm))
      {
         if (debug) std::cout << "   gevonden\n";
         return t->get_nn(nm);
      }
   }
   if (debug) std::cout << "   niet gevonden\n";
   return nullptr;
}

void Toetsen::print()
{
   for (Toets *t: toetsen)
   {
      t->print();
   }
}

// ---------- StemBereik ----------

StemBereik::StemBereik(Noot *lg, Noot *hg) : laag(lg), hoog(hg)
{
}

StemBereik::~StemBereik()
{
   delete laag;
   delete hoog;
}

// ---------- StemBereiken ----------

StemBereiken::StemBereiken()
{
}

StemBereiken::~StemBereiken()
{
   for (int i=0; i<4; i++)
   {
      delete bereiken[i];
   }
}

void StemBereiken::maak_bereiken(Harmonie *harmonie)
{
   NootNaam *nn = harmonie->get_toetsen()->zoek_noot("c");
   if (nn != nullptr)
   {
      nn->print();
   }

   // patroon voor één noot zonder lengte zoals: gis'
   const std::regex   patroon(R"(([a-z]+)((?:\'{0,3}|\,{0,2})))");
   
   // Er zijn 4 stemmen: sop, alt, ten en bas.
   for (int i=0; i<4; i++)
   {
      std::string laag = stembereiken[i][0];
      std::string hoog = stembereiken[i][1];
      
      std::cout << "bereik " << laag << " " << hoog << "\n";
      
      std::string nootnaam_lg;
      std::string acckomma_lg;
      std::string gelezen_lg;
      std::string nootnaam_hg;
      std::string acckomma_hg;
      std::string gelezen_hg;
      
      std::smatch match_laag;
      if (std::regex_search(laag, match_laag, patroon))
      {
         //std::cout << match_laag.size() << " sub-match found!\n";
         //for (auto submatch : match_laag) 
         //{
         //   std::cout << "submatch: " << submatch << "\n";
         //}
         gelezen_lg  = match_laag[0];
         nootnaam_lg = match_laag[1];
         acckomma_lg = match_laag[2];
         std::cout << "laag " << nootnaam_lg << "---" << acckomma_lg << "\n";
      }
      else
      {
         throw ParserError("Interne fout 1, foute naam in bereik");
      }

      std::smatch match_hoog;
      if (std::regex_search(hoog, match_hoog, patroon))
      {
         //std::cout << match_laag.size() << " sub-match found!\n";
         //for (auto submatch : match_laag) 
         //{
         //   std::cout << "submatch: " << submatch << "\n";
         //}
         gelezen_hg  = match_laag[0];
         nootnaam_hg = match_hoog[1];
         acckomma_hg = match_hoog[2];
         std::cout << "hoog " << nootnaam_hg << "---" << acckomma_hg << "\n";
      }
      else
      {
         throw ParserError("Interne fout 2, foute naam in bereik");
      }
      
      int oct_lg = Lied::s_to_octaaf(acckomma_lg);
      int oct_hg = Lied::s_to_octaaf(acckomma_hg);
      
      // Zoek de overeenkomsting nootnamen
      NootNaam *nn_lg = harmonie->get_toetsen()->zoek_noot(nootnaam_lg);
      NootNaam *nn_hg = harmonie->get_toetsen()->zoek_noot(nootnaam_hg);
      
      Trap *tr_lg = harmonie->zoek_trap(nn_lg);
      Trap *tr_hg = harmonie->zoek_trap(nn_hg);
      
      if (nn_lg == nullptr || nn_hg == nullptr)
      {
         throw ParserError("Interne fout 3, foute naam in bereik");
      }

      if (tr_lg == nullptr || tr_hg == nullptr)
      {
         throw ParserError("Interne fout 4, foute naam in bereik");
      }
      
      // Maak 2 noten zonder lengte noch tekst
      Noot *nt_lg = new Noot(tr_lg, oct_lg, 0, gelezen_lg);
      Noot *nt_hg = new Noot(tr_hg, oct_hg, 0, gelezen_hg);
      nt_lg->print();
      nt_hg->print();
      
      StemBereik *bereik = new StemBereik(nt_lg, nt_hg);
      bereiken[i] = bereik;
   }
}

   
// ---------- AkkoordNoot ----------

constexpr int aantal_noten_akkoord = 3;

AkkoordNoot::AkkoordNoot(NootNaam *nt, int rl, int st) : noot(nt), rol(rl), stap(st)
{
}

AkkoordNoot::~AkkoordNoot()
{
   //std::cout << "~AkkoordNoot\n";
}

void AkkoordNoot::set_akkoord(Akkoord *akk)
{
   akkoord = akk;
}

std::string AkkoordNoot::get_naam()
{
   return noot->get_naam();
}

int AkkoordNoot::get_stap() const
{
   return stap;
}



// ---------- AkkoordWijzer ----------

AkkoordWijzer::AkkoordWijzer(Akkoord *akk,  int i) : akkoord(akk), index(i)
{
}

AkkoordWijzer::~AkkoordWijzer()
{
   //std::cout << "~AkkoordWijzer\n";
}

NootNaam *AkkoordWijzer::get()
{
   return akkoord->get(index);
}

int AkkoordWijzer::geti()
{
   return index;
}

void AkkoordWijzer::dec()
{
   index--;
   if (index < 0)
   {
      index = aantal_noten_akkoord - 1;
   }
   if (debug) std::cout << "         index " << index << "\n";
}




// ---------- Akkoord ----------


Akkoord::Akkoord(Trap *tr, AkkoordNoot *gg, AkkoordNoot *tt, AkkoordNoot *kk, int omk) : trap(tr), g(gg), t(tt), omkering(omk), k(kk)
{
}

Akkoord::~Akkoord()
{
   //std::cout << "~Akkoord\n";
   delete g;
   delete t;
   delete k;
}

int Akkoord::get_basisnoot_rang() const
{
   int st = g->get_stap();
   if (st >= aantal_diat_noten)
   {
      st -= aantal_diat_noten;
   }
   return st;
}

bool Akkoord::bevat(NootNaam *nn)
{
   if (nn == g->get_noot())
   {
      return true;
   }
   else
   if (nn == t->get_noot())
   {
      return true;
   }
   else
   if (nn == k->get_noot())
   {
      return true;
   }
   else
   {
      return false;
   }
}

NootNaam *Akkoord::get(int i)
{
   if (i == 0)
   {
      return g->get_noot();
   }
   else
   if (i == 1)
   {
      return t->get_noot();
   }
   else
   if (i == 2)
   {
      return k->get_noot();
   }
   else
   {
      return nullptr;
   }
}

AkkoordWijzer *Akkoord::zoek(NootNaam *nn)
{
   for (int i = 0; i<aantal_noten_akkoord; i++)
   {
      AkkoordNoot *an = nullptr;
      if (i == 0)
      {
         an = g;
      }
      else
      if (i == 1)
      {
         an = t;
      }
      else
      {
         an = k;
      }
      
      if (nn == an->get_noot())
      {
         // gevonden
         return new AkkoordWijzer(this, i);
      }
   }
   return nullptr;
}

Noot *Akkoord::maak_noot(NootNaam *nn, int len)
{
   //Noot(Trap *trp, int oct, int len, std::string tkst);
   Toonaard *toonaard = trap->get_toonaard();
   Trap *trp = toonaard->zoek_trap(nn);
   Noot *nt = new Noot(trp, 0, len, "");
   nt->set_midi(nn->get_midi());
   return nt;
}

void Akkoord::print()
{
   std::string romeins = "";
   
   switch(trap->get_stap())
   {
      case 0:
         romeins = "I";
         break;
      case 1:
         romeins = "II";
         break;
      case 2:
         romeins = "III";
         break;
      case 3:
         romeins = "IV";
         break;
      case 4:
         romeins = "V";
         break;
      case 5:
         romeins = "VI";
         break;
      case 6:
         romeins = "VII";
         break;
   }
   
   std::string omk = "";
   switch (omkering)
   {
      case 0:
         omk = "";
         break;
      case 1:
         omk = "6";
         break;
      case 2:
         omk = "64";
         break;
   }
   
   int ks = k->get_stap();
   while (ks >= aantal_diat_noten)
   {
      ks -= aantal_diat_noten;
   }

   std::cout << "| " << (get_basisnoot_rang()+1) << " | " << romeins << omk << " | " << g->get_naam() << " " << t->get_naam() << " " << k->get_naam()  << " | " << (ks+1) << " |\n";
}

// ---------- Trap ----------

Trap::Trap(std::string nm, int stp, Toonaard *tona, NootNaam *nt, bool diat) : noot(nt), naam(nm), stap(stp), toonaard(tona), diatonic(diat) 
{
}

Trap::~Trap()
{
   //std::cout << "~Trap\n";

   for (Akkoord *akk: akkoorden)
   {
      delete akk;
   }
}

int Trap::get_stap()
{
   return stap;
}

std::string Trap::get_naam()
{
   return naam;
}

NootNaam *Trap::get_noot()
{
   return noot;
}

void Trap::add_akkoord(Akkoord *akk)
{
   akkoorden.push_back(akk);
}

constexpr bool alle_omkeringen = true;

void Trap::maak_akkoorden()
{
   int g = get_stap();
   int t = g; // 2
   int k = g; // 3
   if (is_diatonic())
   {
      t = toonaard->diat_plus(g, 2);
      k = toonaard->diat_plus(g, 4);
   }
   else
   {
      // chords on chromatic steps are always major chords
      t = g + 4;
      k = g + 7;
   }  
   NootNaam *gnn = toonaard->get(g);
   NootNaam *tnn = toonaard->get(t);
   NootNaam *knn = toonaard->get(k);
      
   // basisligging
   AkkoordNoot *gan0 = new AkkoordNoot(gnn, 0, g);
   AkkoordNoot *tan0 = new AkkoordNoot(tnn, 1, t);
   AkkoordNoot *kan0 = new AkkoordNoot(knn, 2, k);
   Akkoord *akk0 = new Akkoord(this, gan0, tan0, kan0, 0);
   gan0->set_akkoord(akk0);
   tan0->set_akkoord(akk0);
   kan0->set_akkoord(akk0);

   if (alle_omkeringen)
   {
      // sixt akkoord
      AkkoordNoot *gan1 = new AkkoordNoot(gnn, 0, g);
      AkkoordNoot *tan1 = new AkkoordNoot(tnn, 1, t);
      AkkoordNoot *kan1 = new AkkoordNoot(knn, 2, k);
      Akkoord *akk1 = new Akkoord(this, tan1, kan1, gan1, 1);
      gan1->set_akkoord(akk1);
      tan1->set_akkoord(akk1);
      kan1->set_akkoord(akk1);

      
      // kwart six akkoord
      AkkoordNoot *gan2 = new AkkoordNoot(gnn, 0, g);
      AkkoordNoot *tan2 = new AkkoordNoot(tnn, 1, t);
      AkkoordNoot *kan2 = new AkkoordNoot(knn, 2, k);
      Akkoord *akk2 = new Akkoord(this, kan2, gan2, tan2, 2);
      gan2->set_akkoord(akk2);
      tan2->set_akkoord(akk2);
      kan2->set_akkoord(akk2);

      add_akkoord(akk1); // sixt
      add_akkoord(akk2); // kwart six
   }
   add_akkoord(akk0); // basis
}

// Zoek een akkoord op deze trap volgens
// de omkering
Akkoord *Trap::zoek_akkoord(int omker)
{
   for (Akkoord *akk: akkoorden)
   {
      if (omker == akk->get_omkering())
      {
         return akk;
      }
   }
   return nullptr;
}

void Trap::iterate(auto fu)
{
   for (Akkoord *akk: akkoorden)
   {
      fu(akk);
   }
}

void Trap::print()
{
   std::cout << "* trap " << naam << "\n";
   std::cout << "* ";
   noot->print();
   std::cout << "\n";
      
   std::cout << "| Rang basis | Functie | Akkoord | Rang hoogste |\n";
   std::cout << "|---|---|---|---|\n";

   for (Akkoord *akk: akkoorden)
   {
      akk->print();
   }
   std::cout << "\n";
}



// ---------- Toonladder ----------

Toonladder::Toonladder()
{
}

Toonladder::~Toonladder()
{
   //std::cout << "~Toonladder\n";
   
   for (Trap *tr: trappen)
   {
      delete tr;
   }
}

Trap *Toonladder::get(int i)
{
   while (i >= aantal_chrom_noten)
   {
      i-= aantal_chrom_noten;
   }
   return trappen[i];
}

// Increment s1 with s2 diatonic steps
int Toonladder::diat_plus(int s1, int s2)
{
   //std::cout << "plus " << s1 << " " << s2 << "\n";
   while (s2 > 0)
   {
      s1++;
      while (s1 >= aantal_chrom_noten)
      {
         s1 -= aantal_chrom_noten;
      }
      Trap *tr = trappen[s1];
      if (tr->is_diatonic())
      {
         s2--;
      }
   }

   //std::cout << "plus ret " << s1 << "\n";
   return s1;
}

void Toonladder::add_trap(Trap *tr)
{
   trappen.push_back(tr);
}
   
void Toonladder::maak_akkoorden()
{
   for (Trap *tr: trappen)
   {
      tr->maak_akkoorden();
   }
}

Trap *Toonladder::zoek_noot(std::string nt)
{
   for (Trap *trp: trappen)
   {
      NootNaam *nnm = trp->get_noot();
      if (nt == nnm->get_naam())
      {
         return trp;
      }
   }
   return nullptr;
}

Trap *Toonladder::zoek_functie(std::string fu)
{
   std::cout << "Toonladder::zoek_functie #" << trappen.size() << "\n";
   int i = 0;
   for (Trap *trp: trappen)
   {
      std::cout << "   trap " << i << "\n";
      if (fu == trp->get_naam())
      {
         return trp;
      }
      i++;
   }
   return nullptr;
}

/**
 * Zoek in deze toonladder een trap
 * waarin een bepaalde nootnaam voorkomt.
 */ 
Trap *Toonladder::zoek_trap(NootNaam *nn)
{
   for (Trap *tr: trappen)
   {
      if (tr->get_noot()  == nn)
      {
         return tr;
      }
   }
   return nullptr;
}

std::vector<Akkoord *> Toonladder::lijst_pasakkoorden(NootNaam *nn)
{
   std::vector<Akkoord *> lijst;
   for (Trap *trap: trappen)
   {
      Akkoord *akk = trap->get_basisakkoord();
      if (akk->bevat(nn))
      {
         std::cout << "      " << akk->get_trap()->get_naam() << "\n";
         lijst.push_back(akk);
      }
   }
   return lijst;
}


void Toonladder::print()
{
   if (lang)
   {
      std::cout << "### Trappen\n\n";

      // toon alle trappen
      for (Trap *tr: trappen)
      {
         std::cout << "iterate1 trap " << tr->get_stap() << "\n"; 
         tr->print();
      }
   }

   // verzamel alle akkoorden
   std::vector<Akkoord *> akkoorden;
      
   for (Trap *tr: trappen)
   {
      std::cout << "iterate2 trap " << tr->get_stap() << "\n"; 
      tr->iterate([&akkoorden](Akkoord *akk)
         {
            akkoorden.push_back(akk);
         }   
      );
   }

   // sort
   std::sort(akkoorden.begin(), akkoorden.end(), [](const Akkoord *lhs, const Akkoord * rhs)
      {
         return lhs->get_basisnoot_rang() < rhs->get_basisnoot_rang(); 
      });      

   std::cout << "### Akkoorden\n\n";
   std::cout << "| Rang basis | Functie | Akkoord | Rang hoogste |\n";
   std::cout << "|---|---|---|---|\n";
   for (Akkoord *akk: akkoorden)
   {
      akk->print();
   }
   std::cout << "\n";
}




// ---------- Toonaard ----------

Toonaard::Toonaard(std::string nm, int ri, bool mnr) : naam(nm), richting(ri), minor(mnr)
{
   toonladder = new Toonladder();
}

Toonaard::~Toonaard()
{
   //std::cout << "~Toonaard\n";
   delete toonladder;
}

std::string Toonaard::get_naam()
{
   return naam;
}

NootNaam *Toonaard::get(int k)
{
   return toonladder->get(k)->get_noot();
}

// This method is not in use
int Toonaard::inc_not_in_use(int j)
{
   j++;
   if (j >= aantal_chrom_noten)
   {
      j = 0;
   }
   return j;
}

int Toonaard::diat_plus(int s1, int s2)
{
   return toonladder->diat_plus(s1, s2);
}

/*
 *       I II III IV V VI VII I
 * major  2  2   1  2 2  2   1
 * minor  2  1   2  2 1  3   1
 * 
 * 
 * 
 */ 
void Toonaard::maak_trappen(Toetsen *tt)
{
   std::string nm = to_lower(naam);
   if (minor)
   {
      // verwijder de m op het einde
      nm.pop_back();
   }
   if (debug) std::cout << "------ maak_trappen " << naam << " " << nm << " ------\n";
   ToetsWijzer *wzr = tt->zoek_noot_tw(nm);
   if (wzr != nullptr)
   {
      int ni = 0;
      NootNaam *nn1 = wzr->get(richting);
      if (debug) std::cout << "      noot 1 diat " << nn1->get_naam() << "\n";
      toonladder->add_trap(new Trap("I", ni++, this, nn1));

      wzr->inc();
      NootNaam *nn2b = wzr->get(richting);
      if (debug) std::cout << "      noot 2b chrom " << nn2b->get_naam() << "\n";
      toonladder->add_trap(new Trap("IIb", ni++, this, nn2b, false));

      wzr->inc();
      NootNaam *nn2 = wzr->get(richting);
      if (debug) std::cout << "      noot 2 diat " << nn2->get_naam() << "\n";
      toonladder->add_trap(new Trap("II", ni++, this, nn2));
      
      if (minor)
      {
         wzr->inc();
      }
      else
      {
         wzr->inc();
         NootNaam *nn3b = wzr->get(richting);
         if (debug) std::cout << "      noot 3b chrom " << nn3b->get_naam() << "\n";
         toonladder->add_trap(new Trap("IIIb", ni++, this, nn3b, false));

         wzr->inc();
      }
      NootNaam *nn3 = wzr->get(richting);
      if (debug) std::cout << "      noot 3 diat " << nn3->get_naam() << "\n";
      toonladder->add_trap(new Trap("III", ni++, this, nn3));
      
      if (minor)
      {
         wzr->inc();
         NootNaam *nn4b = wzr->get(richting);
         if (debug) std::cout << "      noot 4b chrom " << nn4b->get_naam() << "\n";
         toonladder->add_trap(new Trap("IVb", ni++, this, nn4b, false));

         wzr->inc();
      }
      else
      {
         wzr->inc();
      }
      NootNaam *nn4 = wzr->get(richting);
      if (debug) std::cout << "      noot 4 diat " << nn4->get_naam() << "\n";
      toonladder->add_trap(new Trap("IV", ni++, this, nn4));
         
      wzr->inc();
      NootNaam *nn5b = wzr->get(richting);
      if (debug) std::cout << "      noot 5b chrom " << nn5b->get_naam() << "\n";
      toonladder->add_trap(new Trap("Vb", ni++, this, nn5b, false));

      wzr->inc();
      NootNaam *nn5 = wzr->get(richting);
      if (debug) std::cout << "      noot 5 diat " << nn5->get_naam() << "\n";
      toonladder->add_trap(new Trap("V", ni++, this, nn5));
      
      if (minor)
      {
         wzr->inc();
      }
      else
      {
         wzr->inc();
         NootNaam *nn6b = wzr->get(richting);
         if (debug) std::cout << "      noot 6b chrom " << nn6b->get_naam() << "\n";
         toonladder->add_trap(new Trap("VIb", ni++, this, nn6b, false));

         wzr->inc();
      }
      NootNaam *nn6 = wzr->get(richting);
      if (debug) std::cout << "      noot diat 6 " << nn6->get_naam() << "\n";
      toonladder->add_trap(new Trap("VI", ni++, this, nn6));
      
      if (minor)
      {
         wzr->inc();
         NootNaam *nn7bb = wzr->get(richting);
         if (debug) std::cout << "      noot 7bb chrom " << nn7bb->get_naam() << "\n";
         toonladder->add_trap(new Trap("VIIb", ni++, this, nn7bb, false));

         wzr->inc();
         NootNaam *nn7b = wzr->get(richting);
         if (debug) std::cout << "      noot 7bb chrom " << nn7b->get_naam() << "\n";
         toonladder->add_trap(new Trap("VIIb", ni++, this, nn7b, false));

         wzr->inc();
      }
      else
      {
         wzr->inc();
         NootNaam *nn7b = wzr->get(richting);
         if (debug) std::cout << "      noot 7b chrom " << nn7b->get_naam() << "\n";
         toonladder->add_trap(new Trap("VIIb", ni++, this, nn7b, false));

         wzr->inc();
      }
      NootNaam *nn7 = wzr->get(richting);
      if (debug) std::cout << "      noot 7 diat " << nn7->get_naam() << "\n";
      toonladder->add_trap(new Trap("VII", ni++, this, nn7));
      
      delete wzr;
   }
   toonladder->maak_akkoorden();
}

Trap *Toonaard::zoek_noot(std::string nt)
{
   return toonladder->zoek_noot(nt);
}

Trap *Toonaard::zoek_functie(std::string fu)
{
   std::cout << "zoek_functie this " << this << "\n";
   std::cout << "zoek_functie " << toonladder << "\n";
   return toonladder->zoek_functie(fu);
}

Trap *Toonaard::zoek_trap(NootNaam *nn)
{
   return toonladder->zoek_trap(nn);
}

std::vector<Akkoord *> Toonaard::lijst_pasakkoorden(NootNaam *nn)
{
   return toonladder->lijst_pasakkoorden(nn);
}

void Toonaard::print()
{
   std::cout << "## Toonaard " << naam << " " << richting << "\n\n";
   toonladder->print();
}




// ---------- Toonaarden ----------

Toonaarden::Toonaarden()
{
   toonaarden.push_back(new Toonaard("C",    1));

   toonaarden.push_back(new Toonaard("G",    1));
   toonaarden.push_back(new Toonaard("D",    1));
   toonaarden.push_back(new Toonaard("A",    1));
   toonaarden.push_back(new Toonaard("E",    1));
   toonaarden.push_back(new Toonaard("B",    1)); // Ces
   toonaarden.push_back(new Toonaard("Fis",  1)); // Ges
   toonaarden.push_back(new Toonaard("Cis",  1)); // Des
   toonaarden.push_back(new Toonaard("Gis",  1)); // As

   toonaarden.push_back(new Toonaard("F",   -1));
   toonaarden.push_back(new Toonaard("Bes", -1));
   toonaarden.push_back(new Toonaard("Es",  -1));
   toonaarden.push_back(new Toonaard("As",  -1)); // Gis
   toonaarden.push_back(new Toonaard("Des", -1)); // Cis
   toonaarden.push_back(new Toonaard("Ges", -1)); // Fis

   toonaarden.push_back(new Toonaard("Am",   1, true)); // C
   toonaarden.push_back(new Toonaard("Em",   1, true)); // D
   toonaarden.push_back(new Toonaard("Bm",   1, true)); // A
   toonaarden.push_back(new Toonaard("Fism", 1, true)); // E
   toonaarden.push_back(new Toonaard("Cism", 1, true)); // B
   toonaarden.push_back(new Toonaard("Gism", 1, true)); // Fis

   toonaarden.push_back(new Toonaard("Dm",   1, true)); // F
   toonaarden.push_back(new Toonaard("Gm",   1, true)); // Bes
   toonaarden.push_back(new Toonaard("Cm",   1, true)); // Es
   toonaarden.push_back(new Toonaard("Fm",   1, true)); // As
   toonaarden.push_back(new Toonaard("Besm", 1, true)); // Des
   toonaarden.push_back(new Toonaard("Esm",  1, true)); // Ges
}

Toonaarden::~Toonaarden()
{
   //std::cout << "~Toonaarden\n";
   for (Toonaard *to: toonaarden)
   {
      delete to;
   }
}

void Toonaarden::maak_trappen(Toetsen *tt)
{
   for (Toonaard *to: toonaarden)
   {
      to->maak_trappen(tt);
   }
}

Toonaard *Toonaarden::zoek(const std::string tna)
{
   for (Toonaard *ta: toonaarden)
   {
      if (ta->get_naam() == tna)
      {
         return ta;
      }
   }
   return nullptr;
}

/**
 * Zoek een trap die bij een nootnaam past.
 * Er wordt gezocht in alle toonaarden.
 */ 
Trap *Toonaarden::zoek_trap(NootNaam *nn)
{
   for (Toonaard *ta: toonaarden)
   {
      Trap *tr = ta->zoek_trap(nn);
      if (tr != nullptr)
      {
         return tr;
      }
   }
   return nullptr;
}

void Toonaarden::print()
{
   std::cout << "\n# Toonaarden\n\n";
   for (Toonaard *to: toonaarden)
   {
      to->print();
   }
}

// ---------- Harmonie ----------

Harmonie::Harmonie() : toetsen(new Toetsen()), 
                       toonaarden(new Toonaarden()),
                       bereiken(new StemBereiken())
{
   toonaarden->maak_trappen(toetsen);
   bereiken->maak_bereiken(this);
}

Harmonie::~Harmonie()
{
   //std::cout << "~Harmonie\n";
   delete toetsen;
   delete toonaarden;
   delete bereiken;
}

Toonaard *Harmonie::zoek_toonaard(const std::string letters)
{
   return toonaarden->zoek(letters);
}

Trap *Harmonie::zoek_trap(NootNaam *nn)
{
   return toonaarden->zoek_trap(nn);
}

void Harmonie::print()
{
   toetsen->print();
   toonaarden->print();
}

// ----------  Fout ----------

Fout::Fout(int nnr, std::string txt) : nr(nnr), tekst(txt)
{
}

Fout::~Fout()
{
}

int Fout::get_nr()
{
   return nr;
}

std::string Fout::get_tekst()
{
   return tekst;
}

void Fout::print()
{
   std::cout << "   fout " << nr << " " << tekst << "\n";
}

