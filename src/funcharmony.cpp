
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
   { "I",  { "V", "IV" } },
   { "V",  { "I" } },
   { "IV", { "V", "I" } }
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

Trap::Trap(std::string nm, int stp, Toonaard *tona, NootNaam *nt) : noot(nt), naam(nm), stap(stp), toonaard(tona) 
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

void Trap::maak_akkoorden()
{
   int g = get_stap();
   int t = g + 2;
   int k = g + 4;
      
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
   
   add_akkoord(akk0); // basis
   add_akkoord(akk1); // sixt
   add_akkoord(akk2); // kwart six
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
   while (i >= aantal_diat_noten)
   {
      i-= aantal_diat_noten;
   }
   return trappen[i];
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

//constexpr bool lang = false;

void Toonladder::print()
{
   if (lang)
   {
      std::cout << "### Trappen\n\n";

      // toon alle trappen
      for (Trap *tr: trappen)
      {
         tr->print();
      }
   }

   // verzamel alle akkoorden
   std::vector<Akkoord *> akkoorden;
      
   for (Trap *tr: trappen)
   {
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

Toonaard::Toonaard(std::string nm, int ri) : naam(nm), richting(ri)
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

int Toonaard::inc(int j)
{
   j++;
   if (j >= aantal_diat_noten)
   {
      j = 0;
   }
   return j;
}

void Toonaard::maak_trappen(Toetsen *tt)
{
   std::string nm = to_lower(naam);
   if (debug) std::cout << naam << " " << nm << "\n";
   ToetsWijzer *wzr = tt->zoek_noot_tw(nm);
   if (wzr != nullptr)
   {
      NootNaam *nn1 = wzr->get(richting);
      if (debug) std::cout << "      noot 1 " << nn1->get_naam() << "\n";
      toonladder->add_trap(new Trap("I", 0, this, nn1));

      wzr->inc();
      wzr->inc();
      NootNaam *nn2 = wzr->get(richting);
      if (debug) std::cout << "      noot 2 " << nn2->get_naam() << "\n";
      toonladder->add_trap(new Trap("II", 1, this, nn2));
         
      wzr->inc();
      wzr->inc();
      NootNaam *nn3 = wzr->get(richting);
      if (debug) std::cout << "      noot 3 " << nn3->get_naam() << "\n";
      toonladder->add_trap(new Trap("III", 2, this, nn3));
         
      wzr->inc();
      NootNaam *nn4 = wzr->get(richting);
      if (debug) std::cout << "      noot 4 " << nn4->get_naam() << "\n";
      toonladder->add_trap(new Trap("IV", 3, this, nn4));
         
      wzr->inc();
      wzr->inc();
      NootNaam *nn5 = wzr->get(richting);
      if (debug) std::cout << "      noot 5 " << nn5->get_naam() << "\n";
      toonladder->add_trap(new Trap("V", 4, this, nn5));
         
      wzr->inc();
      wzr->inc();
      NootNaam *nn6 = wzr->get(richting);
      if (debug) std::cout << "      noot 6 " << nn6->get_naam() << "\n";
      toonladder->add_trap(new Trap("VI", 5, this, nn6));
         
      wzr->inc();
      wzr->inc();
      NootNaam *nn7 = wzr->get(richting);
      if (debug) std::cout << "      noot 7 " << nn7->get_naam() << "\n";
      toonladder->add_trap(new Trap("VII", 6, this, nn7));
      
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

// ----------  Noot ----------

Noot::Noot(Trap *trp, int oct, int len, std::string tkst) : trap(trp), octaaf(oct), lengte(len), gelezen_tekst(tkst)
{
   midi = trp->get_noot()->get_midi();
   if (oct > 0)
   {
      for (int i=0; i<oct; i++)
      {
         midi += 12;
      }
   }
   else
   if (oct < 0)
   {
      for (int i=0; i< -oct; i++)
      {
         midi -= 12;
      }
   }
}

Noot::~Noot()
{
   //std::cout << "~Noot\n";
}

/*
// probleem, functie mag weg
Noot *Noot::maak_noot(NootNaam *nn, int oct)
{
   Noot *noot = new Noot(nullptr, oct, 0, "");
   
   return noot;
}
 */

std::string Noot::to_s()
{
   std::cout << "   to_s\n";
   if (get_trap() != nullptr)
   {
      std::cout << "      trap " << get_trap()->get_naam() << "\n";
   }
   else
   {
      std::cout << "      trap nullptr\n";
      return gelezen_tekst;
   }
   std::string nootnm = get_trap()->get_noot()->get_naam();
   int oct = get_octaaf();
   if (oct > 0)
   {
      for (int i = 0; i<oct; i++)
      {
         nootnm += "'";
      }
   }
   else
   {
      for (int i = 0; i<-oct; i++)
      {
         nootnm += ",";
      }
   }
   int len = get_lengte();
   switch (len)
   {
      case 1:
         nootnm += "1";
         break;
      case 2:
         nootnm += "2";
         break;
      case 4:
         nootnm += "4";
         break;
      case 8:
         nootnm += "8";
         break;
   }

   return nootnm;
}

void Noot::print()
{
   std::cout << "noot ";
   if (trap != nullptr)
   {
      std::cout << trap->get_noot()->get_naam();
   }
   else
   {
      std::cout << gelezen_tekst;
   }
   std::cout << " oct " << octaaf << " lengte " << lengte << " " << midi << "\n";  
}

// ----------  Functie ----------

Functie::Functie(Trap *trp, std::string tkst) : trap(trp), tekst(tkst)
{
}

Functie::~Functie()
{
   //std::cout << "~Functie\n";
}
Trap       *Functie::get_trap()
{
   return trap;
}

std::string Functie::get_tekst()
{
   return tekst;
}

// ----------  Tel ----------

int Tel::teller = 1;

Tel::Tel() : nr(teller++), lengte(0), functie(nullptr), toonaard(nullptr)
{
   stemmen[0] = nullptr;
   stemmen[1] = nullptr;
   stemmen[2] = nullptr;
   stemmen[3] = nullptr;
}

Tel::~Tel()
{
   //std::cout << "~Tel\n";
   if (stemmen[0] != nullptr) delete stemmen[0];
   if (stemmen[1] != nullptr) delete stemmen[1];
   if (stemmen[2] != nullptr) delete stemmen[2];
   if (stemmen[3] != nullptr) delete stemmen[3];
   delete functie;
   // delete toonaard; weak ptr
}

Toonaard *Tel::get_toonaard()
{
   return toonaard;
}

void Tel::set_toonaard(Toonaard *ta)
{
   toonaard = ta;
}

Noot *Tel::get_stem(int i)
{
   return stemmen[i];
}

void Tel::set_stem(int i, Noot *nt)
{
   stemmen[i] = nt;
}

void Tel::set_functie(Functie *fu)
{
   functie = fu;
}

void Tel::print()
{
   Noot *noot = stemmen[0]; // v1
   int len = noot->get_lengte();
   /*
   std::string naam = noot->get_trap()->get_noot()->get_naam();
   std::cout << "      Tel v1 " << naam << len << "\n";
    */
   std::cout << "      Tel v1 " << (noot != nullptr) << " len " << len << "\n";
   std::cout << "         trap " << (noot->get_trap() != nullptr) << "\n";
   if (noot->get_trap() != nullptr)
   {
      std::cout << "         trap noot " << (noot->get_trap()->get_noot() != nullptr) << "\n";

      std::string naam = noot->get_trap()->get_noot()->get_naam();
      std::cout << "         naam " << naam << len << "\n";
   }
   else
   {
      std::cout << "         naam xx\n";
   }
   if (functie != nullptr)
   {
      std::cout << "         functie "<< functie->get_trap()->get_naam() << "\n";
   }
}

nlohmann::json Tel::to_json()
{
   nlohmann::json tel_js;
   nlohmann::json stemmen_js;
   nlohmann::json fouten_js;

   if (stemmen[0] != nullptr)
   {
      stemmen_js[0] = stemmen[0]->to_s();
   }
   else
   {
      // In this case stemmen[0] is null
      // due to the missing first key
      stemmen_js[0] = "r4";
   }
   stemmen_js[1] = "r4";
   stemmen_js[2] = "r4";
   stemmen_js[3] = "r4";
   tel_js["stemmen"] = stemmen_js;

   tel_js["nr"]      = nr;

   int i = 0;
   for (Fout *f: fouten)
   {
      nlohmann::json fout_js;
      fout_js["nr"]    = f->get_nr();
      fout_js["tekst"] = f->get_tekst();
      fouten_js[i]     = fout_js;
      i++;
   }
   tel_js["fouten"]  = fouten_js;
   return tel_js;
}


// ----------  Maat ----------

Maat::Maat()
{
}

Maat::~Maat()
{
   //std::cout << "~Maat\n";
   for (Tel *tel: tellen)
   {
      delete tel;
   }
}

void Maat::print()
{
   std::cout << "   Maat\n";
   for (Tel *tel: tellen)
   {
      tel->print();
   }
}

nlohmann::json Maat::to_json()
{
   nlohmann::json maat;
   for (Tel *tl: tellen)
   {
      maat.push_back(tl->to_json());
   }
   return maat;
}

void Maat::for_each(std::function<void(Maat *, Tel *)> fu)
{
   for (Tel *tel: tellen)
   {
      fu(this, tel);
   }
}

// ----------  Lied ----------

Lied::Lied(Harmonie *hrm, const std::string fn) : harmonie(hrm), file(new Textfile(fn))
{
}

Lied::~Lied()
{
   //std::cout << "~Lied\n";
   //delete harmonie; // weak
   delete file;
   for (Maat *m: maten)
   {
      delete m;
   }
}

unsigned long Lied::tellen_size()
{
   unsigned long sz = 0;
   
   for (Maat *mt: maten)
   {
      sz += mt->size();
   }
      
   return sz;
}

void Lied::nieuwe_maat()
{
   maten.push_back(new Maat());
}

Tel *Lied::get_tel(unsigned long n)
{
   if (n >= tellen_size())
   {
      if (maten.size() == 0)
      {
         maten.push_back(new Maat());
      }
      Maat *laatste_maat = maten[maten.size()-1];
      assert(laatste_maat != nullptr);
      Tel *tel = new Tel();
      laatste_maat->add(tel);
      return tel;
   }
   else
   {
      unsigned long int i = 0;
      for (Maat *mt: maten)
      {
         for (unsigned int j=0; j<mt->size(); j++)
         {
            if (n == i)
            {
               return mt->get(j);
            }
            i++;
         }
      }
   }
}

int Lied::s_to_octaaf(std::string s)
{
   if (s == "'''")
   {
      return 3;
   }
   else
   if (s == "''")
   {
      return 2;
   }
   else
   if (s == "'")
   {
      return 1;
   }
   else
   if (s == "")
   {
      return 0;
   }
   else
   if (s == ",")
   {
      return -1;
   }
   else
   if (s == ",,")
   {
      return -2;
   }
   else
   if (s == ",,,")
   {
      return -3;
   }
   else
   {
      return 0;
   }
}

int Lied::s_to_lengte(std::string s)
{
   if (s == "1")
   {
      return 1;
   }
   else
   if (s == "2")
   {
      return 2;
   }
   else
   if (s == "4")
   {
      return 4;
   }
   else
   if (s == "8")
   {
      return 8;
   }
   else
   {
      return 4;
   }
}

// Overloop als test de noten van v1.
void Lied::parse_test_v1()
{
   std::cout << "\n";
   for (int i=0; i<file->size(); i++)
   {
      std::string line = file->get(i);
      std::cout << "lijn: " << line;
      
      const std::regex linetype(R"(\[(\w+)\]\s(.*)\n)");
      std::smatch m;

      if (regex_search(line, m, linetype))
      {
         /*
         assert(m.size() == 3);
         assert(m[0].str() == "PQR:2;");                // Entire match
         assert(m[1].str() == "PQR");                   // Substring that matches 1st group
         assert(m[2].str() == "2");                     // Substring that matches 2nd group
         assert(m.prefix().str() == "ABC:1->   ");      // All before 1st character match
         assert(m.suffix().str() == ";;   XYZ:3<<<");   // All after last character match
       */
         /*
         for (const std::string str : m) // Alternatively. You can also do
         {
            std::cout << "   deel: " << str << std::endl;
         }
          */
         
         std::string all  = m[0];
         std::string type = m[1];
         std::string rest = m[2];
         //std::cout << "   all: " << all << std::endl;
         std::cout << "   type: " << type << std::endl;
         std::cout << "   rest: " << rest << std::endl;
         
         if (type == "v1")
         {
            std::cout << "   type v1\n";
            // patroon voor één noot zoals: gis'4
            const std::regex notes(R"((\w+)((?:\'{0,3}|\,{0,2}))((?:1|2|4|8))\s*)");
            
            /*
            std::smatch mno;
            if (regex_search(rest, mno, notes))
            {
               std::cout << "   notes ok #" << mno.size() << "\n";
               std::cout << "   mno[1]:  " << mno[1] << "\n";
               std::cout << "   mno[2]:  " << mno[2] << "\n";
               std::cout << "   mno[3]:  " << mno[3] << "\n";
               
            }
            else
            {
               std::cout << "   notes not ok\n";
            }
             */
            
            // overloop alle noten van een regel, dus multi-match
            std::sregex_iterator it{rest.begin(), rest.end(), notes};
            std::sregex_iterator end;
            while (it != end) 
            {
               //std::cout << "Match";
               
               /*
               int i = 0;
               for (auto mtch : *it) 
               {
                  std::cout << "\n  Submatch: " << mtch;
                  if (i == 1)
                  {
                     std::cout << "letter " << mtch << "\n";
                  }
                  else
                  if (i == 2)
                  {
                     std::cout << "accent " << mtch << "\n";
                  }
                  else
                  if (i == 3)
                  {
                     std::cout << "cijfer " << mtch << "\n";
                  }
                     
                  i++;
               }
                */
               
               // Een noot bestaat uit 3 delen, bv c''2
               std::string letters  = (*it)[1];
               std::string acckomma = (*it)[2];
               std::string cijfer   = (*it)[3];
               
               std::cout << "letters " <<  letters << " acckomma " << acckomma <<  " cijfer "  << cijfer  << "\n";
               it++;
            }
         }
         else
         if (type == "fu")
         {
            std::cout << "   type fu\n";
         }
      }
   }
}

/**
 * maak_noot: false, pas 1, maak geen noten maar wel de tellen en maten
 * maak_noot: true,  pas 2, maak  noten in de bestaande tellen en maten
 */ 
void Lied::parse_v1(const std::string line, bool maak_noot)
{
   std::cout << "\n------- parse v1 ----------\n";
   
   Toonaard * huidige_tna = nullptr;
   if (maak_noot)
   {
      // Haal de eerste toonaard
      if (tellen_size() > 0)
      {
         Tel *tel0 = get_tel(0);
         huidige_tna = tel0->get_toonaard();
         if (huidige_tna == nullptr)
         {
            add_fout(new Fout(1, "De eerste toonaard ontbreekt"));
            throw ParserError("De eerste toonaard ontbreekt");
         }
      }
      else
      {
         add_fout(new Fout(1, "De eerste toonaard ontbreekt"));
         throw ParserError("De eerste toonaard ontbreekt");
      }
      std::cout << "start toonaard " << huidige_tna->get_naam() << "\n";
   }

   // patroon voor één noot zoals: gis'4, zonder maatstreep
   //const std::regex   notes(R"((?:^|\s+)([a-z]+)((?:\'{0,3}|\,{0,2}))((?:1|2|4|8)))");
   
   // patroon voor één noot zoals: gis'4, met maatstreep
   const std::regex   notes(R"((?:(?:^\s*(\|)\s*)|(?:\s*)|(?:\s*(\|)\s*))([a-z]+)((?:\'{0,3}|\,{0,2}))((?:1|2|4|8))(?:\s*(\|)\s*$)?)");

   // dit zijn de indexen:
   //   0: alle gematchte delen samen
   //   1: | aan het begin
   //   2: | tussen de noten
   //   3: letters van de noot
   //   4: , of ' van de noot
   //   5: lengte van de noot
   //   6: | op het einde 
   
   // overloop alle noten van een regel, dus multi-match
   unsigned long i = 0;
   std::sregex_iterator it{line.begin(), line.end(), notes};
   std::sregex_iterator end;
   while (it != end) 
   {
      std::cout << "Match size " << (*it).size() << "\n";

      // test
      int j = 0;
      for (auto s: *it)
      {
         std::cout << "   " << j << ": " << s << "\n";
         j++;
      }
      
      bool pipetussen = false;
      if ((*it)[1] == "|")
      {
         // Een pipe | is gevonden
         std::cout << "pipe | begin gevonden\n";
      }
      else
      if ((*it)[2] == "|")
      {
         // Een pipe | is gevonden
         std::cout << "pipe | tussenin gevonden\n";
         pipetussen = true;
      }
      else
      if ((*it)[6] == "|")
      {
         // Een pipe | is gevonden
         std::cout << "pipe | einde gevonden\n";
      }

      // Een noot is gevonden
      // Een noot bestaat uit 3 delen, bv c''2
      std::string letters  = (*it)[3];
      std::string acckomma = (*it)[4];
      std::string cijfer   = (*it)[5];
               
      std::cout << "i: " << i << " letters " <<  letters << " acckomma " << acckomma <<  " cijfer "  << cijfer  << "\n";
      std::string noottekst = letters + acckomma + cijfer;

      if (maak_noot)
      {
         // Is er op deze tel een nieuwe toonaard?
         if (i > 0 && i < tellen_size())
         {
            //ok, deze tel bestaat al
            Tel *tl = get_tel(i);
            // heeft deze tel een toonaard?
            Toonaard *tna = tl->get_toonaard();
            if (tna != nullptr)
            {
               // wissel van toonaard
               std::cout << "wissel toonaard " << tna->get_naam() << "\n";
               huidige_tna = tna;
            }
         }
      
         bool fout = false;
         std::cout << "zoek noot in toonaard " << huidige_tna->get_naam() << "\n";
         Trap * trap = huidige_tna->zoek_noot(letters);
         if (trap == nullptr)
         {
            fout = true;
            //throw ParserError("Noot niet in toonladder");
         }
         else
         {
            std::cout << "noot trap gevonden\n";
         }

         Noot *noot = new Noot(trap, s_to_octaaf(acckomma), s_to_lengte(cijfer), noottekst);
         Tel  *tl = get_tel(i);
         tl->set_stem(0, noot);
         if (fout)
         {
            std::cout << "noot niet in toonladder\n";
            add_fout(new Fout(tl->get_nr(), "Noot niet in toonladder"));
            fout = false;
         }
      }
      else
      {
         if (pipetussen)
         {
            nieuwe_maat();
         }
         Tel  *tl = get_tel(i);
      }
      i++;
      it++;
   }
}

void Lied::parse_fu(const std::string line)
{
   std::cout << "\n------- parse fu ----------\n";
   
   // Haal de eerste toonaard
   Toonaard * huidige_tna = nullptr;
   if (tellen_size() > 0)
   {
      Tel *tel0 = get_tel(0);
      huidige_tna = tel0->get_toonaard();
      if (huidige_tna == nullptr)
      {
         throw ParserError("De eerste toonaard ontbreekt");
      }
   }
   else
   {
      throw ParserError("De eerste toonaard ontbreekt");
   }
   std::cout << "huidige toonaard " << huidige_tna->get_naam() << "\n";
   
   // patroon voor de functies I, II, III, IV, V, VI en VII
   const std::regex toonaard(R"((?:^|\s+)((?:I|II|III|IV|V|VI|VII)))");
            
   // overloop alle toonaarden van een regel, dus multi-match
   unsigned long i = 0;
   std::sregex_iterator it{line.begin(), line.end(), toonaard};
   std::sregex_iterator end;
   while (it != end) 
   {
      //std::cout << "Match";
               
      // Een toonaard bestaat uit romeinse cijfers
      std::string romcijfer  = (*it)[1];
               
      std::cout << "romeins cijfer " <<  romcijfer << "\n";

      // Is er op deze tel een nieuwe toonaard?
      if (i > 0 && i < tellen_size())
      {
         //ok, deze tel bestaat al
         Tel *tl = get_tel(i);
         // heeft deze tel een toonaard?
         Toonaard *tna = tl->get_toonaard();
         if (tna != nullptr)
         {
            // wissel van toonaard
            std::cout << "wissel toonaard " << tna->get_naam() << "\n";
            huidige_tna = tna;
         }
      }
      
      Tel  *tel  = get_tel(i);
      Trap *trap = huidige_tna->zoek_functie(romcijfer);
      if (trap == nullptr)
      {
         add_fout(new Fout(tel->get_nr(), "functie niet in toonladder"));
         throw ParserError("Functie niet in toonladder");
      }
      std::cout << "functie trap gevonden\n";
      

      Functie *fun = new Functie(trap, romcijfer);
      tel->set_functie(fun);      
      /*
      Noot *noot = new Noot(trap, s_to_octaaf(acckomma), s_to_lengte(cijfer));
      Tel  *tl = get(i);
      tl->set_stem(0, noot);
       */
      
      
      i++;
      it++;
   }
}


void Lied::parse_ke(const std::string line)
{
   std::cout << "\n------- parse ke ----------\n";
   // patroon voor één noot zoals: gis'4
   //const std::regex toonaard(R"(([a-zA-Z]+)(?:\s+))");
   const std::regex toonaard(R"((?:^|\s+)((?:[a-zA-Z]+)|-))");
            
   // overloop alle toonaarden van een regel, dus multi-match
   int i = 0;
   std::sregex_iterator it{line.begin(), line.end(), toonaard};
   std::sregex_iterator end;
   while (it != end) 
   {
      //std::cout << "Match";
               
      // Een toonaard bestaat uit letters [a-zA-Z]
      std::string letters  = (*it)[1];
               
      //std::cout << "letters " <<  letters << " acckomma " << acckomma <<  " cijfer "  << cijfer  << "\n";
      std::cout << "letters " <<  letters << "\n";
      
      if (letters != "-")
      {
         Toonaard *ta = harmonie->zoek_toonaard(letters);
         if (ta != nullptr)
         {
            std::cout << "toonaard gevonden\n";

            Tel *tel = get_tel(i);
            tel->set_toonaard(ta);
         }
         else
         {
            std::cout << "toonaard niet gevonden\n";
         }
      }
      else
      {
         // Deze tel maakt een nieuwe tel
         Tel *tel = get_tel(i);
         tel->set_toonaard(nullptr);
      }

      i++;
      it++;
   }
}

void Lied::parse_linetype(const std::string ltype, bool maak_noot)
{
   std::cout << "\n";
   for (int i=0; i<file->size(); i++)
   {
      std::string line = file->get(i);
      std::cout << "lijn: " << line;
      
      const std::regex linetype(R"(\[(\w+)\]\s(.*)\n)");
      std::smatch m;

      if (regex_search(line, m, linetype))
      {
         std::string all  = m[0];
         std::string type = m[1];
         std::string rest = m[2];
         //std::cout << "   all: " << all << std::endl;
         std::cout << "   type: " << type << std::endl;
         std::cout << "   rest: " << rest << std::endl;
         
         if (type == ltype)
         {
            if (type == "v1")
            {
               parse_v1(rest, maak_noot);
            }
            else
            if (type == "fu")
            {
               parse_fu(rest);
            }
            else
            if (type == "ke")
            {
               parse_ke(rest);
            }
         }
      }
   }
}

void Lied::parse()
{
   parse_linetype("v1", false);
   parse_linetype("ke", false);
   parse_linetype("v1", true);
   parse_linetype("fu", false);
}

void Lied::print()
{
   std::cout << "++++ Lied ++++\n";
   for (Maat *mt: maten)
   {
      mt->print();
   }
   for (Fout *fout: fouten)
   {
      fout->print();
   }
}


void Lied::add_fout(Fout *f)
{
   fouten.push_back(f);
}

nlohmann::json Lied::to_json()
{
   nlohmann::json lied_js;
   nlohmann::json maten_js;
   for (Maat *m: maten)
   {
      maten_js.push_back(m->to_json());
   }
   lied_js["maten"] = maten_js;

   nlohmann::json fouten_js = nlohmann::json::array();
   int i = 0;
   for (Fout *f: fouten)
   {
      nlohmann::json fout_js;
      fout_js["nr"]    = f->get_nr();
      fout_js["tekst"] = f->get_tekst();
      fouten_js[i]     = fout_js;
      i++;
   }
   lied_js["fouten"]  = fouten_js;
   
   return lied_js;
}

void Lied::to_ly()
{
   inja::Environment env;

   nlohmann::json lied_js = to_json();
   nlohmann::json data;
   data["lied"] = lied_js;

   // Or directly read a template file
   inja::Template temp = env.parse_template("./templates/muziek.tly");
   env.write(temp, data, "./muziek.ly");
}

void Lied::for_each(std::function<void(Maat *, Tel *)> fu)
{
   std::cout << "maat\n";
   for (Maat *maat: maten)
   {
      maat->for_each(fu);
   }
}

void Lied::maak_stemmen()
{
   for_each([](Maat *m, Tel *t)
   {
      std::cout << "   tel\n";
   });
}

