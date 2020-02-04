// $Id: util.cpp,v 1.14 2019-10-08 14:01:38-07 - - $

#include <cstdlib>
#include <unistd.h>

using namespace std;

#include "util.h"
#include "debug.h"

bool want_echo() {
   constexpr int CIN_FD {0};
   constexpr int COUT_FD {1};
   bool cin_is_not_a_tty = not isatty (CIN_FD);
   bool cout_is_not_a_tty = not isatty (COUT_FD);
   DEBUGF ('u', "cin_is_not_a_tty = " << cin_is_not_a_tty
          << ", cout_is_not_a_tty = " << cout_is_not_a_tty);
   return cin_is_not_a_tty or cout_is_not_a_tty;
}

string exec::execname_; // Must be initialized from main().
int exec::status_ = EXIT_SUCCESS;

string basename (const string &arg) { 
   return arg.substr (arg.find_last_of ('/') + 1);
}

void exec::execname (const string& argv0) {
   execname_ = basename (argv0);
   cout << boolalpha;
   cerr << boolalpha;
   DEBUGF ('u', "execname = " << execname_);
}

void exec::status (int status) {
   if (status_ < status) status_ = status;
}

string append_from(long unsigned int start, vector<string> words) {
   string final_string = "";
   for (long unsigned int vector_index = start;
      vector_index < words.size();vector_index++) {
      final_string = final_string.append(words.at(vector_index));
      if (vector_index < words.size()-1) {
         final_string = final_string.append(" ");
      }
   }
   return final_string;
}

wordvec split (const string& line, const string& delimiters) {
   wordvec words;
   size_t end = 0;

   // Loop over the string, splitting out words, and for each word
   // thus found, append it to the output wordvec.
   for (;;) {
      size_t start = line.find_first_not_of (delimiters, end);
      if (start == string::npos) break;
      end = line.find_first_of (delimiters, start);
      words.push_back (line.substr (start, end - start));
   }
   DEBUGF ('u', words);
   return words;
}

//note: code taken from
//https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

ostream& complain() {
   exec::status (EXIT_FAILURE);
   cerr << exec::execname() << ": ";
   return cerr;
}

