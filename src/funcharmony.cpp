
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


NootNamen::NootNamen()
{
}


// ---------- Wit ----------

Wit::Wit(NootNaam *nn) : nootnaam(nn)
{
}

bool Wit::heeft_naam(std::string nm)
{
   return nm == nootnaam->get_naam();
}

NootNaam *Wit::get(int ri)
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

Toets *Toetsen::get(int i)
{
   return toetsen[i];
}

ToetsWijzer *Toetsen::zoek_noot(std::string nm)
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

void Toetsen::print()
{
   for (Toets *t: toetsen)
   {
      t->print();
   }
}


// ---------- AkkoordNoot ----------

AkkoordNoot::AkkoordNoot(NootNaam *nt, int rl, int st) : noot(nt), rol(rl), stap(st)
{
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
   ToetsWijzer *wzr = tt->zoek_noot(nm);
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

void Toonaarden::print()
{
   std::cout << "\n# Toonaarden\n\n";
   for (Toonaard *to: toonaarden)
   {
      to->print();
   }
}

// ---------- Harmonie ----------

Harmonie::Harmonie() : toetsen(new Toetsen()), toonaarden(new Toonaarden())
{
   toonaarden->maak_trappen(toetsen);
}

Harmonie::~Harmonie()
{
}

Toonaard *Harmonie::zoek_toonaard(const std::string letters)
{
   return toonaarden->zoek(letters);
}

void Harmonie::print()
{
   toetsen->print();
   toonaarden->print();
}

// ----------  Noot ----------

Noot::Noot(Trap *trp, int oct, int len) : trap(trp), octaaf(oct), lengte(len)
{
}

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

// ----------  Tel ----------

Tel::Tel() : lengte(0), functie(nullptr), toonaard(nullptr)
{
   stemmen[0] = nullptr;
   stemmen[1] = nullptr;
   stemmen[2] = nullptr;
   stemmen[3] = nullptr;
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
   std::cout << "         trap noot " << (noot->get_trap()->get_noot() != nullptr) << "\n";

   std::string naam = noot->get_trap()->get_noot()->get_naam();
   std::cout << "         naam " << naam << len << "\n";
}

// ----------  Maat ----------

Maat::Maat()
{
}

void Maat::print()
{
   std::cout << "   Maat\n";
   for (Tel *tel: tellen)
   {
      tel->print();
   }
}

// ----------  Lied ----------

Lied::Lied(Harmonie *hrm, const std::string fn) : harmonie(hrm), file(new Textfile(fn))
{
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
            throw ParserError("De eerste toonaard ontbreekt");
         }
      }
      else
      {
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

      
         std::cout << "zoek noot in toonaard " << huidige_tna->get_naam() << "\n";
         Trap * trap = huidige_tna->zoek_noot(letters);
         if (trap == nullptr)
         {
            throw ParserError("Noot niet in toonladder");
         }
         std::cout << "noot trap gevonden\n";
      

         Noot *noot = new Noot(trap, s_to_octaaf(acckomma), s_to_lengte(cijfer));
         Tel  *tl = get_tel(i);
         tl->set_stem(0, noot);
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
      
      
      Trap * trap = huidige_tna->zoek_functie(romcijfer);
      if (trap == nullptr)
      {
         throw ParserError("Functie niet in toonladder");
      }
      std::cout << "functie trap gevonden\n";
      
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
}

void Lied::to_ly_voice(std::ofstream &lyf, int i)
{
   //lyf << "c''4 d''4 e''4 f''4";
   lyf << "   ";
   for (unsigned int j = 0; j<tellen_size(); j++)
   {
      Tel *tel = get_tel(j);
      Noot *noot = tel->get_stem(i);
      if (noot != nullptr)
      {
         lyf << noot->to_s() << " ";
      }
   }
   lyf << "\n";
}

/*
void Lied::to_ly()
{
    std::ofstream lyf;
    lyf.open("muziek.ly");
    std::cout << lyf.is_open() << "\n";
    lyf <<
R"EOF(\version "2.24.4"

\include "roman_numeral_analysis_tool.ily"

\pointAndClickOff

\header
{
   title =  "oef sopr gegeven2"
}

\layout
{
   \context 
   { 
      \Score
      skipBars = ##t
   }
}

global =
{
   \time 4/4 \key g \major
}

VoiceOne =
{
   \override Score.BarNumber.break-visibility = #end-of-line-invisible
   \set Score.currentBarNumber = 1
   % Print a bar number every second measure
   \set Score.barNumberVisibility = #(every-nth-bar-number-visible 1)

   \clef "treble"
   \global
)EOF";
   nlohmann::json data;
   data["v1"] = "g''4 a''4 g''4 e''4";
   inja::render_to(lyf, "{{ v1 }}", data );
   //to_ly_voice(lyf, 0);

   lyf <<
R"EOF(}

VoiceTwo =
{
   \clef "treble" 
   \global
)EOF";
   
   to_ly_voice(lyf, 1);

   lyf <<
R"EOF(}

VoiceThree =
{
   \clef "bass"
   \global 
)EOF";
   
   to_ly_voice(lyf, 2);

   lyf <<
R"EOF(}

VoiceFour =
{
   \clef "bass"
   \global
)EOF";
   
   to_ly_voice(lyf, 3);

   lyf <<
R"EOF(}

analysis = \lyricmode
{
   %\override LyricText.self-alignment-X = #-0.6
   %\offset StanzaNumber.X-offset #-3
   %\set stanza  = #"G:"

   % For bare Roman numerals, \rN simply outputs the string.
   \markup \rN { I }
   \markup \rN { I }
   \markup \rN { V }
   \markup \rN { I }
}


% The score definition
\score
{
   <<
      \new ChoirStaff
      <<
         \context Staff = "1"
         << 
            \mergeDifferentlyDottedOn\mergeDifferentlyHeadedOn
            \context Voice = "VoiceOne" {  \voiceOne \VoiceOne }
            \context Voice = "VoiceTwo" {  \voiceTwo \VoiceTwo }
         >>
         \context Staff = "2" 
         <<
            \mergeDifferentlyDottedOn\mergeDifferentlyHeadedOn
            \context Voice = "VoiceThree" {  \voiceOne \VoiceThree }
            \context Voice = "VoiceFour"  {  \voiceTwo \VoiceFour }
            \context Lyrics \lyricsto "VoiceFour" { \analysis  }
         >>
      >>
        
   >>
   \layout {}
   % To create MIDI output, uncomment the following line:
   %  \midi {\tempo 4 = 100 }
}
)EOF";
    
    lyf.close();
}
 */

void Lied::to_ly()
{
   inja::Environment env;

   nlohmann::json data;
   data["name"] = "world";

   // Or directly read a template file
   inja::Template temp = env.parse_template("./templates/muziek.tly");
   env.write(temp, data, "./muziek.ly");
}
