#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

#include "textfile.h"

Textfile::Textfile(const std::string &fn) : filename(fn)
{
   readfile();
}

std::string Textfile::remove_comment(std::string line)
{
   auto pos = line.find_first_of(';');
   if (pos == std::string::npos)
   {
      return line;
   }
   else
   {
      // skip ; and the following characters
      return line.substr(0, pos);
   }
}


void Textfile::readfile()
{
   std::string   line;
   std::ifstream fin;
   std::cout << "readfile " << filename;

   fin.open(filename);
   std::cout << "na open";

   if(!fin.is_open())
   {
      std::cout << "Error at reading file!\n";
      //exit(1);
   }
   else
   {
      std::cout << "open ok";

      getline(fin, line);
      while(!fin.eof())
      {
         std::cout << "line read " << line;
         lines.push_back(remove_comment(line) + "\n");

         getline(fin, line);
      }
      std::cout << "Read " << lines.size();
   }
}

void Textfile::print()
{
   std::cout << "---- lines ----\n";
   std::cout << "   #" << lines.size() << "\n";
   for (std::string line: lines)
   {
      std::cout << "line " << line << "\n";
   }
   std::cout << "---- ------ ----\n";
}


