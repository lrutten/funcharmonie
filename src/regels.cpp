
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

void Lied::vul_rusten(Tel *t)
{
   t->set_stem(1, new Rust(t->get_lengte()));
   t->set_stem(2, new Rust(t->get_lengte()));
   t->set_stem(3, new Rust(t->get_lengte()));
}

/**
 * Maak de noten (Noot) voor alt, ten en bas.
 * Plaats ze onder elkaar en vul in in de tel.
 */ 
void Lied::zet_stemmen_in_tel(Tel *t, Functie *fu, Akkoord *akk, Noot *n_sop, NootNaam *nn_alt, NootNaam *nn_ten, NootNaam *nn_bas)
{
   std::cout << "zet_stemmen_in_tel\n";
   
   // Maak de noten met behulp van de namen
   Noot *n_alt = akk->maak_noot(nn_alt, t->get_lengte());
   Noot *n_ten = akk->maak_noot(nn_ten, t->get_lengte());
   Noot *n_bas = akk->maak_noot(nn_bas, t->get_lengte());

   std::cout << "noten gemaakt\n";

   std::cout << "               noot sop " << n_sop->to_s() << "\n";
   std::cout << "               noot alt " << n_alt->to_s() << "\n";
   std::cout << "               noot ten " << n_ten->to_s() << "\n";
   std::cout << "               noot bas " << n_bas->to_s() << "\n";
               
   // Schik de noten onder elkaar.
   // Momenteel mogen de noten nog niet samenvallen
   n_alt->onder(n_sop);
   n_ten->onder(n_alt);
   n_bas->onder(n_ten);

   std::cout << "               noot sop " << n_sop->to_s() << "\n";
   std::cout << "               noot alt " << n_alt->to_s() << "\n";
   std::cout << "               noot ten " << n_ten->to_s() << "\n";
   std::cout << "               noot bas " << n_bas->to_s() << "\n";

   // Verplaats in octaven
   std::string plmn = fu->get_plusmin();
   if (plmn != "")
   {
      char pm_alt = ' ';
      char pm_ten = ' ';
      char pm_bas = ' ';
      int len = plmn.size();

      pm_bas = plmn[len - 1];
      if (pm_bas == '+')
      {
         n_bas->verhoog();
      }
      if (pm_bas == '-')
      {
         n_bas->verlaag();
      }
      if (len >= 2)
      {
         pm_ten = plmn[len - 2];
         if (pm_ten == '+')
         {
            n_ten->verhoog();
         }
         if (pm_ten == '-')
         {
            n_ten->verlaag();
         }
      }
      if (len >= 3)
      {
         pm_alt = plmn[len - 0];
         if (pm_alt == '+')
         {
            n_alt->verhoog();
         }
         if (pm_alt == '-')
         {
            n_alt->verlaag();
         }
      }
   }
   
   // Vul de stemmen
   t->set_stem(1, n_alt);
   t->set_stem(2, n_ten);
   t->set_stem(3, n_bas);
}

/*
 * Maak de alt, ten en bas stemmen.
 */
void Lied::maak_stemmen()
{
   Tel    *vorige_tel     = nullptr;
   Ligging vorige_ligging = lig_geen;
   
   for_each([this, &vorige_tel, &vorige_ligging](Maat *m, Tel *t)
   {
      std::cout << "   tel " << t->get_lengte() << "\n";
      ANoot *anoot = t->get_stem(0); // sop stem

      assert(anoot != nullptr);

      /*
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
       */
      
      bool verbinding_mag = true;
      Functie *functie = t->get_functie();
      std::string tel_naam;
      std::string vtel_naam;
      if (vorige_tel != nullptr)
      {
         Functie *vorige_functie = vorige_tel->get_functie();
         if (functie != nullptr && vorige_functie != nullptr)
         {
            tel_naam  = functie->get_trap()->get_naam();
            vtel_naam = vorige_functie->get_trap()->get_naam();
            std::cout << "      verbinding " << vtel_naam << "-" << tel_naam << "\n"; 
            
            std::set<std::string> naar = functies[vtel_naam];
            if (tel_naam != vtel_naam && !naar.contains(tel_naam))
            {
               verbinding_mag = false;
            }
         }
      }
      
      if (!verbinding_mag)
      {
         // deze verbinding is niet toegestaan
         
         nlohmann::json data;
         data["fu1"] = vtel_naam;
         data["fu2"] = tel_naam;
         std::string fout_tekst = 
         inja::render("De verbinding {{ fu1 }}-{{ fu2 }} is verboden", data);
         add_fout(new Fout(t->get_nr(), fout_tekst));
         
         add_fout(new Fout(t->get_nr(), "Deze verbinding is verboden"));
         vul_rusten(t);         
      }
      else
      if (functie != nullptr)
      {
         std::cout << "      functie " << functie->get_tekst() << " " 
            << functie->get_kwartsixt() << " " 
            << ew_to_s(functie->get_engwijd()) << "\n";

         // Haal de ligging van deze functie
         // en pas die eventueel aan   
         Ligging ew = functie->get_engwijd();
         if (ew == lig_geen)
         {
            // gebruik de laatst ingestelde eng of wijd
            ew = vorige_ligging;
         }
         
         Trap *trap = functie->get_trap();
         
         assert(trap != nullptr);

         if (trap != nullptr)
         {
            std::cout << "         trap " << trap->get_naam() << "\n";
         }

         // Gebruik voorlopig omkering 0
         Akkoord *akk = trap->zoek_akkoord(0); // 0: basisligging
         
         
         Noot *n_sop = dynamic_cast<Noot *>(anoot);
         
         // n_sop met een Noot zijn en geen Rust
         // In de sopraanpartij kan geen rust ingegeven worden.
         assert(n_sop != nullptr);

         if (akk != nullptr)
         {
            // Er is een akkoord in de trap van deze functie.
            bool in_akk = akk->bevat(n_sop->get_trap()->get_noot());
            if (in_akk)
            {
               // De sopraannoot komt voor in het akkoord.
               std::cout << "         in_akk\n";

               // Wijs de noot van de sopraan aan.
               AkkoordWijzer *wijzer = akk->zoek(n_sop->get_trap()->get_noot());
               NootNaam *nn_sop = wijzer->get();
               NootNaam *nn_alt = nullptr;
               NootNaam *nn_ten = nullptr;

               /*
                  oude test
                          (trap->get_naam() == "I"  ||
                           trap->get_naam() == "IV" ||
                           trap->get_naam() == "V"))
                */
               
               // Is dit een sixt akkoord van de hoofdtrappen I IV V?               
               if (functie->get_kwartsixt() == "6" && trap->is_main())
               {
                  // Hier mag de terts niet verdubbeld worden.
                  std::cout << "         sixt hoofdtrap\n";
                  
                  int sop_index = wijzer->geti();
                  constexpr int gr = 0;
                  constexpr int te = 1;
                  constexpr int kw = 2;
                  
                  // Hier mag de terts geen tweemaal voorkomen.
                  if (sop_index == te)
                  {
                     // De sopraannoot is de terts van het 6 akkkoord.
                     // Dit is verboden.
                     add_fout(new Fout(t->get_nr(), "De sopraannoot mag de terts van het 6 sixtakkoord niet zingen"));
                  }
                  
                  // Kies gr of kw voor alt en ten.
                  if (sop_index == gr)
                  {
                     std::cout << "sop gr\n";
                     switch (ew)
                     {
                        //case lig_geen:
                        case lig_eng:
                           std::cout << "   eng\n";
                           nn_alt = akk->get(kw);
                           nn_ten = akk->get(gr);
                           break;
                        case lig_wijd:
                           std::cout << "   wijd\n";
                           nn_alt = akk->get(gr);
                           nn_ten = akk->get(kw);
                           break;
                        case lig_xwijd:
                           std::cout << "   xwijd\n";
                           nn_alt = akk->get(kw);
                           nn_ten = akk->get(kw);
                           break;
                     }
                  }
                  else
                  {
                     std::cout << "sop kw\n";
                     switch (ew)
                     {
                        //case lig_geen:
                        case lig_eng:
                           std::cout << "   eng\n";
                           nn_alt = akk->get(gr);
                           nn_ten = akk->get(kw);
                           break;
                        case lig_wijd:
                           std::cout << "   wijd\n";
                           nn_alt = akk->get(kw);
                           nn_ten = akk->get(gr);
                           break;
                        case lig_xwijd:
                           std::cout << "   xwijd\n";
                           nn_alt = akk->get(gr);
                           nn_ten = akk->get(gr);
                           break;
                     }
                  }
               }
               else
               {
                  // Hier hebben we een akkoord in de grondligging of kwartsix op elke
                  // trap, ook trap IIb, of een sixt akkoord op de neventrappen of IIb.
                  if (trap->get_naam() == "IIb")
                  {
                     std::cout << "         napolitaans sixt\n";
                  }
                  else
                  {
                     std::cout << "         geen sixt hoofdtrap\n";
                  }

                  if (trap->is_diatonic() && !trap->is_main())
                  {
                     std::cout << "         neventrap\n";
                     if (functie->get_dominant() == "d")
                     {
                     std::cout << "         dominant\n";
                     }
                  }
                  // Leg alt en ten vast voor akkoorden in grondligging en 64 akkoorden
                  // Omkering 0 en 1, dus bv I en I64
                  wijzer->dec();
                  if (ew == lig_wijd)
                  {
                     // extra stap omlaag
                     wijzer->dec();
                  }
                  nn_alt = wijzer->get();
                  wijzer->dec();
                  if (ew == lig_wijd)
                  {
                     // extra stap omlaag
                     wijzer->dec();
                  }
                  nn_ten = wijzer->get();
               }

               // De bovenstaande code heeft de alt en ten stemmen bepaald
               // volgens de omkering I, I6 of I64. Nu nog de bas.

               // Kies de bas volgens de omkering van het akkoord.
               NootNaam *nn_bas = nullptr;
               if (functie->get_kwartsixt() == "")
               {
                  nn_bas = akk->get(0); // Neem de basisnoot van het akkoord
               }
               else
               if (functie->get_kwartsixt() == "6")
               {
                  nn_bas = akk->get(1); // Neem de terts van het akkoord
               }
               else
               if (functie->get_kwartsixt() == "64")
               {
                  nn_bas = akk->get(2); // Neem de kwint van het akkoord
               }

                
               // toon de noten voordat ze onder elkaar staan
               std::cout << "            sop    " << nn_sop->get_naam() << "\n";
               std::cout << "            alt    " << nn_alt->get_naam() << "\n";
               std::cout << "            ten    " << nn_ten->get_naam() << "\n";
               std::cout << "            bas    " << nn_bas->get_naam() << "\n";
               
               // Dit is een test om te zien of het enharmonisch verlagen van de nootnamen werkt.
               // Zo wordt cis --> des.
               /*
               std::cout << "            sop lw " << nn_sop->force_lower()->get_naam() << "\n";
               std::cout << "            alt lw " << nn_alt->force_lower()->get_naam() << "\n";
               std::cout << "            ten lw " << nn_ten->force_lower()->get_naam() << "\n";
               std::cout << "            bas lw " << nn_bas->force_lower()->get_naam() << "\n";
                */

               if (n_sop->is_laag() && trap->get_naam() == "IIb")
               {
                  std::cout << "            sop is laag in trap IIb\n";
                  nn_alt = nn_alt->force_lower();
                  nn_ten = nn_ten->force_lower();
                  nn_bas = nn_bas->force_lower();
               }
               else
               if (n_sop->is_hoog())
               {
                  std::cout << "            sop is hoog\n";
               }
               else
               {
                  std::cout << "            sop is laag noch hoog\n";
               }

               // toon de verlaagde noten voordat ze onder elkaar staan
               std::cout << "            sop2    " << nn_sop->get_naam() << "\n";
               std::cout << "            alt2    " << nn_alt->get_naam() << "\n";
               std::cout << "            ten2    " << nn_ten->get_naam() << "\n";
               std::cout << "            bas2    " << nn_bas->get_naam() << "\n";

               zet_stemmen_in_tel(t, functie, akk, n_sop, nn_alt, nn_ten, nn_bas);
               
               
               // Onthoud de ligging indien die expliciet voorkomt
               // in deze functie
               if (functie->get_engwijd() != lig_geen)
               {
                  vorige_ligging = functie->get_engwijd();
               }
               else
               {
                  // deze tel heeft geen ligging
                  if (vorige_tel == nullptr)
                  {
                     // Dit is de eerste tel en die
                     // specifieert een ligging
                     // Ga verder met eng en geef een foutmelding.
                     vorige_ligging = lig_eng;
                     add_fout(new Fout(t->get_nr(), "Vermeld eng of wijd in de eerste noot"));
                  }
                  else
                  if (vorige_tel->get_functie()->get_kwartsixt() == "6")
                  {
                     // Met een kwartsixt akkoord in de vorige tel 
                     // moet deze tel ook een eng of wijd vermelding hebben.
                     // Ga verder met eng en geef een foutmelding.
                     vorige_ligging = lig_eng;
                     add_fout(new Fout(t->get_nr(), "Vermeld eng of wijd na een 6 akkoord"));
                  }
               }
            }
            else
            {
               // De sopraan noot komt niet voor in het akkoord.
               // Foutafhandeling komt hier.
               
               std::cout << "         !in_akk\n";
               // Bij fout, plaats rusten in alt, ten en bas
               vul_rusten(t);
               
               Toonaard *toonaard = n_sop->get_trap()->get_toonaard();
               std::vector<Akkoord *> akkoorden = toonaard->lijst_pasakkoorden(n_sop->get_trap()->get_noot());
               
               // Maak een lijst van akkoorden die wel passen
               //std::vector<Akkoord *> akkoorden = n_sop->get_trap()->lijst_pasakkoorden(n_sop->get_trap()->get_noot());

               nlohmann::json lijst_js = nlohmann::json::array();
               int k = 0;
               for (Akkoord *akk: akkoorden)
               {
                  std::cout << "         wel akkoord " << akk->get_trap()->get_naam() << "\n";
                  lijst_js[k] = akk->get_trap()->get_naam();
                  k++;
               }
               nlohmann::json data;
               data["akkoorden"] = lijst_js;
               std::string fout_tekst = 
                  inja::render("Functie past niet bij sopraan, wel {% for akk in akkoorden %}{{ akk }} {% endfor%}", data);
               
               add_fout(new Fout(t->get_nr(), fout_tekst));
            }
         }
         
         // Onthoud de vorige tel
         vorige_tel = t;
      }
      else
      {
         std::cout << "      geen functie\n";
         vul_rusten(t);
      }
   }); // for-each
}

