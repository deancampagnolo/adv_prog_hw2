// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

inode_state::~inode_state() {
   root -> invalidate();
   cout<<"placeholder"<<endl;
}
const string& inode_state::prompt() const { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   if (state.root == NULL) {
      return out;
   }
   out << "inode_state: root = " << state.root->get_inode_nr()
       << ", cwd = " << state.cwd->get_inode_nr() << ", cwd parent = "
       << state.cwd->get_parent().lock()->get_inode_nr();
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

void inode::set_base_file_inode(inode_ptr current_inode,
   file_type type) {
   contents->set_current_inode(current_inode);
   if (type == file_type::DIRECTORY_TYPE) {
      contents->insert_default_dirents();
   }
 }

 void inode::set_plain_file_contents(const wordvec& newdata) {
    contents->writefile(newdata);
 }

void inode::invalidate() {
   contents = nullptr;
   //contents->get_dirents
}

file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&, wordvec&) {
   throw file_error ("is a " + error_file_type());
}

map<string,inode_ptr> base_file::get_dirents() {
   throw file_error ("is a " + error_file_type());
}

void base_file::insert_default_dirents() {
   throw file_error ("is a " + error_file_type());
}

size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   return data;
}

void plain_file::writefile (const wordvec& content) {
   cout<<"we be out here writing: "<< append_from(0,content)<<endl;
   data = content;
}

directory::~directory() {
   cout<<"destroyed"<<endl;
   for (auto pair : dirents) {
      if (!(pair.first == "." || pair.first == "..")) {
         pair.second->invalidate();
      }
      pair.second = nullptr;
   }
}

void directory::insert_default_dirents() {
   auto temp_current_shared = current_inode.lock();
   auto temp_parent_shared = temp_current_shared->get_parent().lock();
   cout<<"insert_default_dirents ran"<<endl;
   dirents.insert({".",temp_current_shared});
   dirents.insert({"..",temp_parent_shared});
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   cout<<"we here"<<endl;
   inode_ptr new_inode_ptr = 
      make_shared<inode>(file_type::DIRECTORY_TYPE);
   new_inode_ptr->set_parent(current_inode);
   new_inode_ptr->set_base_file_inode(new_inode_ptr,
      file_type::DIRECTORY_TYPE);

   dirents.insert({dirname,new_inode_ptr});
   // TODO(me) need to sort dirents
   DEBUGF ('i', dirname);
   return new_inode_ptr;
}

inode_ptr directory::mkfile (const string& filename,
   wordvec& contents) {

   inode_ptr new_inode_ptr =
      make_shared<inode>(file_type::PLAIN_TYPE);
   new_inode_ptr->set_parent(current_inode);
   new_inode_ptr->set_base_file_inode(new_inode_ptr,
      file_type::PLAIN_TYPE);
   new_inode_ptr->set_plain_file_contents(contents);

   dirents.insert({filename, new_inode_ptr});
   DEBUGF ('i', filename);
   return nullptr;
}

