
#include <iostream>
#include <vector>

#include <QApplication>

#include "funcharmony.h"
#include "mainwindow.h"  // NOLINT(build/include_subdir) INCLUDE_NO_DIR

/*
 * Steps
 *    1 read file
 *    2 parse file
 *    3 make voices
 *    4 write .ly
 *    5 execute lilypond
 */ 
constexpr int steps = 5;

class Main
{
private:
   Harmonie *harmonie;

public:
   Main();
   ~Main();
   void open(std::string filenm);
   void print();
};

Main::Main()
{
   harmonie = new Harmonie();
}

Main::~Main()
{
   delete harmonie;
}

void Main::open(std::string filenm)
{
   Lied *lied = new Lied(harmonie, filenm);

   if (steps <= 1)
   {
      std::cout << "steps 1\n";

      delete lied;
      return;
   }
   
   try
   {
      lied->parse();
      lied->print();
      
      if (steps <= 2)
      {
         std::cout << "steps 2\n";

         delete lied;
         return;
      }
      lied->maak_stemmen();
      //lied->to_ly();
   }
   catch(ParserError fout)
   {
      std::cout << "Parser fout: " << fout.get_melding() << "\n";
   }
   std::string fn_noext = filenm.substr(0, filenm.rfind("."));
   lied->to_ly(fn_noext + ".ly");

   system("pwd");
   std::string lily = "lilypond --svg -dresolution=600 -dcrop=#t " + fn_noext + ".ly";
   system(lily.c_str());
   std::string mv = "mv " + fn_noext + ".cropped.svg " + fn_noext + ".svg";
   system(mv.c_str());

   delete lied;
}

void Main::print()
{
   harmonie->print();
}

int main(int argc, char** argv)
{
   Main *mn = new Main();
   mn->print();
   
   // lees lied
   if (argc >= 2)
   {
      mn->open(argv[1]);
   }
   else
   {
      mn->open("oef41.hrm");
   }
   delete mn;

   /*
   poorten();

   qRegisterMetaType<std::chrono::microseconds>("std::chrono::microseconds");

   QApplication app(argc, argv);

   MainWindow mainWindow;
   mainWindow.show();

   return app.exec();
    */

   return 0;
}
