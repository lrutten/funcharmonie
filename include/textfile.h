#ifndef TEXTFILE_H
#define TEXTFILE_H

#include <string>
#include <vector>
#include <tuple>
#include <memory>

class Textfile
{
private:
   std::string              filename;
   std::vector<std::string> lines;

public:
   explicit Textfile(const std::string &fn);
   void readfile();
   void print();
   std::string remove_comment(std::string line);
   int size()
   {
      return lines.size();
   }
   std::string get(int i)
   {
      return lines[i];
   }
};

#endif

