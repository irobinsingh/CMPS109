/// $Id: commands.cpp,v 1.11 2014-06-11 13:49:31-07 - - $
#include "commands.h"
#include "debug.h"

commands::commands(): map ({
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"    , fn_rmr    },
}){}

command_fn commands::at (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   command_map::const_iterator result = map.find (cmd);
   if (result == map.end()) {
      throw yshell_exn (cmd + ": no such function");
   }
   return result->second;
}


void fn_cat (inode_state& state, const wordvec& words){
   if(words.size()>1){
      for(auto word = words.begin()+1;word !=words.end(); word++){
         bool r = ((*word)[0] == '/');
        cout << state.readfile( split(*word,"/") ,r) << endl;
      }
   }else{
   throw yshell_exn("cat:: no file given");
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   auto dir = state.get_contents();
   if(words.size()>1){
        bool r = false;
        if(words[1][0] == '/') {r = true;}
        wordvec pathname = split(words[1], "/") ;
        try{
            state.set_cwd(pathname,r);
        }catch (yshell_exn){
         throw yshell_exn("cd :: no such directory");
        }
   }else{// cd no arguements?

   }

}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   for (auto it =words.begin()+1 ; it != words.end();it++){
      cout << *it << " ";
   }
   cout<< "\n";
}

void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   wordvec temp{"","/"};
   if(words.size()>1){
   int status = atoi(words[1].c_str());
   exit_status::set(status);
   }
   fn_rmr(state,temp);
   throw ysh_exit_exn();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   bool r = false;
   string orig = pathname(state.get_contents().at("."),
            state.get_root()->get_inode_nr());
   if (words.size() > 1 ){
      for(auto word = words.begin()+1;word !=words.end(); word++){
          if((*word)[0] == '/') {r = true;}
          state.set_cwd(split(*word,"/"),r);
          print_dir(state);
          state.set_cwd(split(orig,"/"),true);
      }
   }else{
   print_dir(state);
   }
   
}



void fn_lsr (inode_state& state, const wordvec& words){
   wordvec path{"", pathname(state.get_contents().at("."), state.get_root()->get_inode_nr())} ;
   fn_ls(state,words);
   fn_cd(state,words);
   for(auto elem : state.get_contents()){
      if(elem.second->get_type()==DIR_INODE && elem.first != "." && 
         elem.first != ".."){
         wordvec temp {"",elem.first};
         fn_lsr(state,temp);
      }
   }
   fn_cd(state,path);

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}


void fn_make (inode_state& state, const wordvec& words){
   bool r = false; wordvec data ;
   if(words[1][0] == '/') {r = true;}
   wordvec pathname = split(words[1], "/") ;
   for(auto it=words.begin()+2; it!=words.end();it++){
      data.push_back(*it);
   }
   state.add_file(data,pathname,r);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   bool r = false; 
   for(auto word = words.begin()+1;word !=words.end(); word++){
   if(words[1][0] == '/') {r = true;}
   wordvec pathname = split(words[1], "/") ;
   try{state.add_directory(pathname,r);
   }catch(yshell_exn e){
      string what = e.what();
      throw yshell_exn("mkdir::" + what);
   }
}
}

void fn_prompt (inode_state& state, const wordvec& words){
   string str {};
   for(auto it = words.begin()+1;it!= words.end();++it){
      str = str + *it + " ";
   }
   state.set_prompt(str);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words){
   cout<< pathname(state.get_contents().at("."),
      state.get_root()->get_inode_nr()) << endl;

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}


void fn_rm (inode_state& state, const wordvec& words){
   //cannot be . and .. or a directory   
   for(auto word = words.begin()+1;word !=words.end(); word++){
     bool r = false;
      if((*word)[0] == '/') {r = true;}
      wordvec pathname = split(*word, "/") ;
      try{
      state.remove(pathname,r,true);
      }catch(yshell_exn e){
       string ew = e.what();
       throw yshell_exn("rm::" + ew );
      }
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   if(words.size() >1){
   bool r = (words[1][0] == '/');
   wordvec path{"", pathname(state.get_contents().at("."), state.get_root()->get_inode_nr())};
   fn_cd(state,words);
   for(auto dir : state.get_contents()){
         wordvec temp {"",dir.first};
      if(dir.second->get_type()==DIR_INODE && dir.first != ".." &&
         dir.first != "."){
         fn_rmr(state,temp);
      }
   }
   wordvec removal_list {};
   for(auto dir : state.get_contents()){
       removal_list.push_back(dir.first);
   }
   for(auto dir : removal_list){
      try{
      state.remove( split(dir,"/"), false, false);
      }catch(yshell_exn e){
         string ew = e.what();
         throw yshell_exn("rmr::" + ew);
      }
   }
   fn_cd(state,path);
   state.remove(split(words[1],"/") ,r,false);
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

string pathname(inode_ptr ip,int root_nr){
      int my_inode_nr = ip->get_inode_nr();
      vector<string> pathname;
      while(my_inode_nr != root_nr){
         ip = ip->get_contents().at("..");
         for(auto& elem: ip->get_contents()){
            if(elem.second->get_inode_nr() == my_inode_nr){
               pathname.insert(pathname.begin(),elem.first);
            }
         }
         my_inode_nr = ip->get_inode_nr();

      }
      string path_str = "/";
      for(auto& str : pathname){
         path_str = path_str  + str+ "/";
      }
      return path_str;

}


void print_dir(inode_state& state){
   cout<< pathname(state.get_contents().at("."),
      state.get_root()->get_inode_nr())<< ":" << endl;
   for(auto elem : state.get_contents()){
      cout << "\t" <<elem.second->get_inode_nr() <<" " 
      << elem.second->size() << " " 
      << elem.first << " " 
      << endl;
   }
   cout << endl;
}
