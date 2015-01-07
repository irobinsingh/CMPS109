// $Id: main.cpp,v 1.6 2014-07-09 11:50:34-07 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_pair = xpair<const string,string>;
using str_str_map = listmap<string,string>;

using wordvec = vector<string> ;
void do_op(string s,str_str_map& sm){
  //cout << s << endl;
  //cout << "------------------------------"<< endl;  
  // # comment
  if(s[s.find_first_not_of(" ")]=='#'){return;}
               

  // key 
  if(s.find_first_of("=") == string::npos){
       if(!s.empty()){
       auto elem = sm.find(s);
       if(elem != sm.end()){
          cout << *elem << endl;
       }else{
          cout<<s << ": key not found" << endl;
       }
      }
  }



  //= or = values
  else if(s.find_first_of("=")==0){
    if(s.find_first_not_of("= ")!=string::npos){
      //cout <<"substr" << s.substr(s.find_first_of("=")+1,string::npos)                    <<endl;
      auto values =
      split(s.substr(s.find_first_of("=")+1,string::npos)," ");
      auto it = values.begin();
      for(;it !=values.end();it++){
          //cout << "value " << *it << endl;
          for(auto itm= sm.begin();itm!=sm.end();++itm){
            if(*it == (*itm).second)cout << *itm <<endl;
          }
                    }
     }else{
        cout << sm;
     }
  }

              


  // = is second
    else {
      auto key_string = s.substr(s.find_first_not_of("= "),s.find_first_of("= "));
      if(s.find_first_not_of("= ",s.find_first_of("= "))==string::npos){
         cout << "delete" <<endl;
         auto elem = sm.find(key_string);
         if(elem != sm.end()){
             sm.erase(elem);
         }else{
             cout << "cannot delete not in map" << endl;
         }
      }else{
          auto value_string=s.substr(s.find_first_not_of("= ",s.find_first_of("= ")) , string::npos);
          cout << key_string  << " = " << value_string << endl;
          auto elem = sm.find(key_string);
          if(elem != sm.end()){
             elem->second = value_string;
          }else{
             sm.insert({key_string,value_string});
          }
      }
      return;
  }
                

         
}
void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            traceflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
}


int main (int argc, char** argv) {
   //sys_info::set_execname (argv[0]);
   scan_options (argc, argv);
   fstream in_file; string s{};
  //option files
  for (int argi = 1; argi < argc; ++argi) {
        str_str_map sm {}; int count(0);
        in_file.open(argv[argi]);
        if(in_file.is_open()){
           while(getline(in_file,s)){
            cout << argv[argi] <<": " << ++count <<": "<< s<<endl;
            do_op(s,sm);  
          }
        }else{
          cout << argv[argi] << ":  file does not exist" << endl;

        }
      in_file.close();
  }

  if(argc < 2){
     char c_line[1024];
     int count(0);
     str_str_map sm {};
     for(;;){
       if(cin.eof()) break;
       cin.getline (c_line,1024);
       string s_line (c_line);
       cout << "-: " << ++count<<": " << s_line<<endl; 
       do_op(s_line,sm);
      }
   }
   return EXIT_SUCCESS;


}


