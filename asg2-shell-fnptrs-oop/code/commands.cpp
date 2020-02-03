// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $

#include "commands.h"
#include "debug.h"
#include "map"

command_hash cmd_hash {
   {"#"     , fn_comment},
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
   {"rm"    , fn_rm    }
   
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   auto result = cmd_hash.find (cmd);
   if (cmd.at(0) == '#') { 
      result = cmd_hash.find("#");
   }
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_comment (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << "this was ignored because its a comment!" << endl;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << state.get_cwd_ptr()->get_base_file_ptr()->get_dirents().
      find(words.at(1))->second->get_base_file_ptr()->readfile()<<endl;
}

void fn_cd (inode_state& state, const wordvec& words){
   map<string,inode_ptr> the_dirents = state.get_cwd_ptr()->
      get_base_file_ptr()->get_dirents();
   if (words.size() > 1) {
      the_dirents.find(words.at(1));
      state.set_cwd(the_dirents.find(words.at(1))->second);
   } else {
      state.set_cwd(state.get_root_ptr());
   }
   //TODO(me) maybe it isn't words at 1
   //
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   cout << state << endl;

   map<string,inode_ptr> the_dirents = state.get_cwd_ptr()->
      get_base_file_ptr()->get_dirents();
   cout<<&the_dirents<<endl;
   cout<<"length " << the_dirents.size()<<endl;
   for (auto pair : the_dirents) {
      cout<<"1: "<<pair.first<<" 2: "<<pair.second<<"\n"<<endl;
   }
   
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   wordvec the_content(words.begin()+2, words.end());
   state.get_cwd_ptr()->get_base_file_ptr()->mkfile(words.at(1),
      the_content);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   state.get_cwd_ptr()->get_base_file_ptr()->mkdir(words.at(1));
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout <<"printing vector " <<words << endl;
   
   state.set_prompt_(append_from(1,words));
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout<<state<<endl;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words); 
   inode_ptr target = state.get_cwd_ptr()->get_base_file_ptr()->get_dirents().
      find(words.at(1))->second;

   if (target->get_base_file_ptr()->get_identity() == 
      file_type::PLAIN_TYPE ||target->get_base_file_ptr()->
      get_dirents().size() <= 2) {

      state.get_cwd_ptr()->get_base_file_ptr()->remove(words.at(1));
   } else {
      cout<< "you cant do that!"<< endl;   
   }
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   state.get_cwd_ptr()->get_base_file_ptr()->remove(words.at(1));
}

