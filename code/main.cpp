// $Id: main.cpp,v 1.13 2021-02-01 18:58:18-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>
#include <cassert>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>


using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

void catfile(istream& infile, const string& filename) {
  regex comment_regex {R"(^\s*(#.*)?$)"};
  regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
  regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};

  str_str_map test;

  cout << filename << endl;
  for(;;) {
    string line;
    getline (cin, line);
    if (infile.eof()) break;
    cout << "input: \"" << line << "\"" << endl;
    smatch result;
    if (regex_search (line, result, comment_regex)) {
       cout << "Comment or empty line." << endl;
    }else if (regex_search (line, result, key_value_regex)) {
       cout << "key  : \"" << result[1] << "\"" << endl;
       cout << "value: \"" << result[2] << "\"" << endl;
       if(result[1] == "" && result[2] == "") {
         for(auto it = test.begin();it != test.end();++it) {
           cout << *it << endl;
         }
       } else if(result[1] == "") {
         test.find_key(result[2]);
       } else if(result[2] == "") {
         auto it = test.find(result[1]);
         if(it != test.end()) {
           cout << "erase : " << result[1] << endl;
           test.erase(it);
         }
       } else {
         str_str_pair pair (result[1], result[2]);
         cout << pair << endl;
         test.insert(pair);
       }
    }else if (regex_search (line, result, trimmed_regex)) {
       cout << "query: \"" << result[1] << "\"" << endl;
       auto it = test.find(result[1]);
       if(it != test.end()) {
         cout << "value: \"" << (*it).second << "\"" << endl;
       } else {
         cout << result[1] << ": key not found" << endl;
       }
    }else {
       assert (false and "This can not happen.");
    }
  }
}

int main (int argc, char** argv) {
  int status = 0;
  string progname(basename(argv[0]));
  vector<string> filenames(&argv[1], &argv[argc]);
  if(filenames.size()==0) filenames.push_back("-");
  for(const auto& filename: filenames) {
    if(filename == "-") catfile(cin,filename);
    else {
      ifstream infile (filename);
      if(infile.fail()) {
        status = 1;
        cerr << progname << ": " << filename <<": " 
        << strerror(errno) << endl;
      } else {
        catfile(infile,filename);
        infile.close();
      }
    }
  }
  cout << "EXIT_SUCCESS" << endl;
  return status;
}

