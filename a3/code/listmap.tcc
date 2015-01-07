// $Id: listmap.tcc,v 1.5 2014-07-09 11:50:34-07 - - $

#include "listmap.h"
#include "trace.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//
//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}



//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
      auto it = begin();
      for(;it!= end() || nullptr;){
        //cout << &*it << endl;
        it = erase(it);
      }
}

//
// listmap::empty()
//
template <typename Key, typename Value, class Less>
bool listmap<Key,Value,Less>::empty() const {
   return anchor_.next == anchor_.prev;
}

//
// listmap::iterator listmap::begin()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::begin() {
   return iterator (anchor_.next);
}

//
// listmap::iterator listmap::end()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::end() {
   return iterator (anchor());
}


//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
 //cout << "-----------------------" <<endl;
 //  TRACE ('l', &pair << "->" << pair);
   auto itor = begin();
   if(begin() == end() ) {
      node* n = new node(begin().get_node(),begin().get_node()->next,pair);
      begin().get_node()->next->prev = n;
      begin().get_node()->next = n;
      return itor;
   }else{
   for(;itor!=end();++itor){
       //cout << *itor << endl; 
       if(less(pair.first,(*itor).first)){
         //cout << *itor<<endl;
         node* n = new node(itor.get_node(),(--itor).get_node(),pair);
         (++itor).get_node()->prev->next =n ;
         (itor).get_node()->prev = n;
         return itor;
       }  
   }
   node* n = new node(itor.get_node(),(--itor).get_node(),pair);
   (++itor).get_node()->prev->next =n ;
   (itor).get_node()->prev = n;
   return itor;
   }  
}
//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   TRACE ('l', that);
    auto itor = begin();
   for(; itor != end();++itor){
      if(itor->first == that){
         //cout << *itor << endl;
         break;
      }
   }
   
   return itor;
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   TRACE ('l', &*position);
   auto curr = position.get_node();
   //cout << curr->value <<endl;
   ++position;
   // cout << "moving" <<endl;
   //if(curr == anchor()) cout << "deleting anchor" <<endl;
   //if(curr->next == nullptr){cout << "next is null" << endl; return nullptr;}
   //if(curr->prev == nullptr){cout << "prev is null" << endl; return nullptr;}
   curr->prev->next = curr->next;
   curr->next->prev = curr->prev;
   delete curr;
   return position;
}

//
// listmap::operator<<(listmap)
//
template <typename Key, typename Value, class Less>
std::ostream& operator<< (std::ostream& out, 
        listmap<Key,Value,Less>& map){
    auto it = map.begin();
    for(;it!=map.end();++it){
      out << *it << endl;
    }
    return out;
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   TRACE ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   TRACE ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   TRACE ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   TRACE ('l', where);
   where = where->prev;
   return *this;
}


//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}

template <typename Key, typename Value, class Less>
typename  listmap<Key,Value,Less>::node*
listmap<Key,Value,Less>::iterator::get_node(){
      return  where;
}
