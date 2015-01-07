// $Id: xless.h,v 1.3 2014-04-24 18:02:55-07 - - $

#ifndef __XLESS_H__
#define __XLESS_H__

//
// We assume that the type type_t has an operator< function.
//
template <typename Type>
struct xless {
   bool operator() (const Type& left, const Type& right) const {
      return left < right;
   }
};
/*
template <typename Type, class Less=xless<Type>>
void less (const Type& left, const Type& right){
     return left < right;
}

template <typename Type, class Less=xless<Type>>
void equal (const Type& left, const Type& right){
     return (not left < right) and (not right < left);
}

template <typename Type, class Less=xless<Type>>
void greater (const Type& left, const Type& right){
     return (not left < right) and (not equal(left,right));
}
*/
#endif
