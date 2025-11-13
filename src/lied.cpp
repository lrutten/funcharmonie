
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


// ----------  ANoot ----------

ANoot::ANoot(int len) : lengte(len)
{
}

ANoot::~ANoot()
{
}

// ----------  Noot ----------

Noot::Noot(Trap *trp, int oct, int len, std::string tkst) : ANoot(len), trap(trp), octaaf(oct), gelezen_tekst(tkst)
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

bool Noot::is_rust()
{
   return false;
}

std::string Noot::to_s()
{
   //std::cout << "   to_s\n";
   if (get_trap() != nullptr)
   {
      //std::cout << "      trap " << get_trap()->get_naam() << "\n";
   }
   else
   {
      //std::cout << "      trap nullptr\n";
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

// ----------  Rust -------------

Rust::Rust(int len) : ANoot(len)
{
}

Rust::~Rust()
{
}

bool Rust::is_rust()
{
   return true;
}

Trap *Rust::get_trap()
{
   return nullptr;
}

std::string Rust::to_s()
{
   //std::cout << "   Rust::to_s\n";
   std::string nootnm = "r";
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

void Rust::print()
{
   std::cout << "rust ";
   std::cout << " lengte " << lengte << "\n";  
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

ANoot *Tel::get_stem(int i)
{
   return stemmen[i];
}

void Tel::set_stem(int i, ANoot *nt)
{
   stemmen[i] = nt;
}

void Tel::set_functie(Functie *fu)
{
   functie = fu;
}

void Tel::print()
{
   ANoot *noot = stemmen[0]; // v1
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
         tl->set_lengte(s_to_lengte(cijfer));  // de tel krijgt de lengte van de v1 noot
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
   std::cout << "   parse line '" << line << "'\n";
   
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
   const std::regex toonaard(R"((?:^|\s+)((?:IV|III|II|I|VII|VI|V)))");
            
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
            std::cout << "   wissel toonaard " << tna->get_naam() << "\n";
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
      std::cout << "   functie trap gevonden\n";
      

      Functie *fun = new Functie(trap, romcijfer);
      tel->set_functie(fun);
      
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
      std::cout << "   tel " << t->get_lengte() << "\n";
      ANoot *anoot = t->get_stem(0); // sop stem

      assert(anoot != nullptr);

      // if enkel voor test
      if (anoot != nullptr && !anoot->is_rust())
      {
         // Elke noot die geen rust is, heeft een trap.
         std::cout << "      noot " << anoot->get_trap()->get_naam() << "\n";
      }
      else
      {
         std::cout << "      geen noot of wel rust\n";
      }
      
      Functie *functie = t->get_functie();
      if (functie != nullptr)
      {
         std::cout << "      functie " << functie->get_tekst() << "\n";
         Trap *trap = functie->get_trap();
         
         assert(trap != nullptr);

         if (trap != nullptr)
         {
            std::cout << "         trap " << trap->get_naam() << "\n";
         }

         // Gebruik voorlopig omkering 0
         Akkoord *akk = trap->zoek_akkoord(0); // 0: basisligging
         
         
         // Hier is een functiefout!!!!!!
         // Inlezen functies is fout
         
         Noot *noot = dynamic_cast<Noot *>(anoot);
         if (noot != nullptr && akk != nullptr)
         {
            bool in_akk = akk->bevat(noot->get_trap()->get_noot());
            if (in_akk)
            {
               // Wijs de noot van de sopraan aan.
               AkkoordWijzer *wijzer = akk->zoek(noot->get_trap()->get_noot());
               std::cout << "         in_akk\n";

               NootNaam *nn_sop = wijzer->get();
               std::cout << "            sop " << nn_sop->get_naam() << "\n";

               wijzer->dec();
               NootNaam *nn_alt = wijzer->get();
               std::cout << "            alt " << nn_alt->get_naam() << "\n";

               wijzer->dec();
               NootNaam *nn_ten= wijzer->get();
               std::cout << "            ten " << nn_ten->get_naam() << "\n";
               
               // Maak de noten met behulp van de namen
               Noot *n_alt = nn_alt->maak_noot();
               Noot *n_ten = nn_ten->maak_noot();
            }
            else
            {
               // Foutafhandeling komt hier.
               
               std::cout << "         !in_akk\n";
            }
         }
         
         
         
         // Enkel voor de test, plaats rusten in alt, ten en bas
         t->set_stem(1, new Rust(t->get_lengte()));
         t->set_stem(2, new Rust(t->get_lengte()));
         t->set_stem(3, new Rust(t->get_lengte()));
      }
      else
      {
         std::cout << "      geen functie\n";
         
         t->set_stem(1, new Rust(t->get_lengte()));
         t->set_stem(2, new Rust(t->get_lengte()));
         t->set_stem(3, new Rust(t->get_lengte()));
      }
   });
}

