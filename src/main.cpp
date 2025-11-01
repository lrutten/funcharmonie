
#include <iostream>
#include <vector>

#include <QApplication>

#include "funcharmony.h"
#include "mainwindow.h"  // NOLINT(build/include_subdir) INCLUDE_NO_DIR


class Main
{
private:
   Harmonie *harmonie;

public:
   Main();
   ~Main();
   void open();
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

void Main::open()
{
   Lied *lied = new Lied(harmonie, "vb1.hrm");

   try
   {
      lied->parse();
      lied->print();
      lied->to_ly_template();
   }
   catch(ParserError fout)
   {
      std::cout << "Parser fout: " << fout.get_melding() << "\n";
   }
}

void Main::print()
{
   harmonie->print();
}

int main(int argc, char** argv)
{
   Main *mn = new Main();
   //mn->print();
   mn->open();

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
