// $Id: bigint.cpp,v 1.61 2014-06-26 17:06:06-07 - - $

#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
using namespace std;
using digit_t = unsigned char;
#include "bigint.h"
#include "debug.h"

bigint::bigint (const string& that) {
   isnegative = false; //set number to positive and then check
   auto itor = that.cbegin();
   auto ritor = that.rbegin();
   auto rend  = that.rend();
   if (itor != that.cend() and *itor == '_') {
      isnegative = true;
      rend--;
   }
   // push new number in place note this reverse num
   while (ritor != rend ) {
    //cout << *ritor;
    num.push_back( *ritor++ - '0'); 
    }
   //cout << endl;
   DEBUGF ('~', this << " -> " << long_value)
}

bigint::bigint(int that){
   isnegative = false;
   int place = 10;
   if(that == 0 ) {num.push_back(0); return;}
   if(that < 0 ) {isnegative =true; that *= -1;}
    while(that > 0 ){
       int dig = that % place;
       that -= dig; place *= 10;
        num.push_back(static_cast<digit_t>(dig)); 
    }
}
bigint::bigint(long that){
   isnegative = false;
   long place = 10.0;
   if(that == 0 ) {num.push_back(0); return;}
   if(that < 0 ) {isnegative =true; that *= -1.0;}
    while(that > 0 ){
       long dig = (that % place);
       that -= dig; 
       dig /= (place/10.0);
       place *= 10.0;
       dig = labs(dig);
       num.push_back(static_cast<digit_t>(dig)); 
    }
    this->strip();
}

bigint::bigint(const bigint& that){
      isnegative = that.isnegative;
      long_value = that.long_value;
      num.clear();
      num = that.num;
}




bigint operator+ (const bigint& left, const bigint& right) {
       bigint b{};
       if (left.isnegative == right.isnegative){
          b = do_add(left,right);
          b.isnegative = left.isnegative;
	        return b;
       }        
         b=(left < right) ? do_sub(left,right) : do_sub(right,left);
         if(left < right) b.isnegative = left.isnegative;
         return b;
}

bigint operator- (const bigint& left, const bigint& right) {
       bigint b{};
       if (left.isnegative != right.isnegative){
          b = do_add(left,right);
          b.isnegative = left.isnegative;
          return b;
       }        
         b= ( left > right) ? do_sub(left,right) : do_sub(right,left);
         if(left > right){ 
          b.isnegative = left.isnegative;
        }else{
          b.isnegative = !right.isnegative;
        }
         return b;
}

bigint operator+ (const bigint& right) {
   return  + right;
}


bigint operator- (const bigint& right) {
   return  - right;
}

void bigint::strip(){
  auto ritor = num.rbegin();
  for(;ritor != num.rend() ;ritor++){
    if(*ritor != 0) break;
    num.pop_back();
  }

}

// itorates backwards over intvec and multply by ten to 
//correct for digit placement
long bigint::to_long() const {
   bigint min(numeric_limits<long>::min());
   bigint max(numeric_limits<long>::max());
   cout << max << "  " << *this << endl;
   if (*this < min or *this > max)
               throw range_error ("bigint__to_long: out of range");
   auto ritor = this->num.rbegin();
   int long_value_r {};
   while(ritor != this->num.rend()) long_value_r = 10*long_value_r + *ritor;
   return long_value_r;
}

//do_add
bigint do_add(const bigint& left,const bigint& right){
       //cout << "do add has been called "<<endl;
       bigint b{}; int carry_over =0;
       auto itl = left.num.begin(); auto itr = right.num.begin();
       int art_size = (left.num.size()>right.num.size()) ?
                     left.num.size() : right.num.size();
       for(int i =0; i<art_size+1;i++){
          int l = carry_over; int r =0;
          if(itl != left.num.end()){ l += *itl; ++itl;}
          if(itr != right.num.end()){r += *itr; ++itr;}
         // cout << "left equals "<<l << " right equals "<<r ;
          b.num.push_back( (l+r) %10); carry_over = (l+r)/10;
          //cout << " = " << (l+r)%10  <<"  " << (l+r)/10 << endl ;

       }
       b.strip();  
       return b;
}
//do_sub left should always be bigger than right
bigint do_sub(const bigint& left,const bigint& right){
       //cout << left << " - " << right;
       bigint b{}; int carry_over =0;
       auto itl = left.num.begin(); auto itr = right.num.begin();
       int art_size=left.num.size();
       for(int i =0; i<art_size;i++){
          int l = 0; int r =0;
          if(itl != left.num.end()){l=*itl; ++itl;}
          if(itr != right.num.end()){r = *itr;++itr;}
          if(l - r- carry_over <0){
            l+=10; 
            b.num.push_back(l-r-carry_over);
            carry_over=1; 
          }else{
          b.num.push_back (l-r-carry_over);
          carry_over = 0;
          }
      }
       b.strip();
       //cout << " = " << b <<endl;
       return b;

}


//do_mult
bigint do_mult(const bigint& left,const bigint& right){
  bigint b{}; int d_p {};
  b.num.resize(left.num.size()+right.num.size() +1);
  auto itl = left.num.begin(); 
  for(int i = 0; i<static_cast<int>(left.num.size());i++){
      int carry_over = 0;
      auto itr = right.num.begin();
      for(int j =0; j < static_cast<int>(right.num.size());j++){
         d_p = b.num[i+j] + (*itl)*(*itr) + carry_over;
         //cout << static_cast<int>(*itl)<< static_cast<int>(*itr) << endl;
         b.num[i+j] = d_p%10;
         carry_over = d_p/10;
         ++itr;
      }
      b.num[i+right.num.size()] = carry_over;
      ++itl;
   }
   b.strip();
   return b;
}
bool abs_less (const bigint& left, const bigint& right) {
    if(left.num.size() != right.num.size())cout << "not same size" << endl;
    auto itl = left.num.rbegin();
    auto itr = right.num.rbegin();
    for (int i = 0; i < (int)left.num.size() ; i++){
        if(*itl < *itr){
          return true;
        }else if(*itl > *itr){
          return false;
        }
        ++itl; ++itr;
    }
    return false;
}

//
// Multiplication algorithm.
//
bigint operator* (const bigint& left, const bigint& right) {
   bigint b = do_mult(left,right);
   if(left.isnegative == right.isnegative){
      b.isnegative = false;
   }else{
      b.isnegative = true;
   }
   return b;
}

//
// Division algorithm.
//

void multiply_by_2 (bigint& b) {
   b.num = do_add(b,b).num;
   b.strip();
}

void divide_by_2 (bigint& b) {
  auto itor = b.num.rbegin(); auto end = b.num.rend();
  int carry_over = 0;
  for(;itor != end ;itor++){
      if(*itor%2==1){
         *itor = *itor/2 +carry_over; 
         carry_over = 5;
      }else{
        *itor = *itor/2 +carry_over; 
         carry_over = 0;
      }
   }
   b.strip();
}



bigint::quot_rem divide (const bigint& left, const bigint& right) {
    bigint zero(0);
   if (right == zero) throw domain_error ("divide by 0");
   if (right == zero) throw domain_error ("bigint::divide");
   bigint divisor(right); bigint power_of_2 (1);
   bigint quotient(0);    bigint rem(left); 
   while (divisor < rem ){
      multiply_by_2 (divisor);
      multiply_by_2 (power_of_2);
   }
   while (zero < power_of_2) {  
   if (not (rem < divisor)) {
         rem = rem - divisor;
         quotient = quotient + power_of_2;
      }
      divide_by_2 (divisor);
      divide_by_2 (power_of_2);
   }
   return {quotient, rem};
}

bigint operator/ (const bigint& left, const bigint& right) {
    bigint neg_one(-1);
    if(left.isnegative && !right.isnegative){
      bigint b = divide (left*neg_one, right).first;
      b.isnegative = true;
      return b;
   }
   if(right.isnegative && !left.isnegative){
      bigint b = divide (left, right*neg_one).first;
      b.isnegative = true;
      return b;
   }
   if(right.isnegative && left.isnegative){
      bigint b = divide (left*neg_one, right*neg_one).first;
      b.isnegative = false;
      return b;
   }else{
      bigint b = divide (left, right).first;
      b.isnegative = false;
      return b;

   }

}

bigint operator% (const bigint& left, const bigint& right) {
   bigint neg_one(-1);
    if(left.isnegative && !right.isnegative){
      bigint b = divide (left*neg_one, right).second;
      b.isnegative = true;
      return b;
   }
   if(right.isnegative && !left.isnegative){
      bigint b = divide (left, right*neg_one).second;
      b.isnegative = true;
      return b;
   }
   if(right.isnegative && left.isnegative){
      bigint b = divide (left*neg_one, right*neg_one).second;
      b.isnegative = false;
      return b;
   }else{
      bigint b = divide (left, right).second;
      b.isnegative = false;
      return b;

   }
}

bool operator== (const bigint& left, const bigint& right) {
   if(left > right){
    return false;
   }else if(left < right){
    return false;
   }else{
    return true;
   }
}

bool operator< (const bigint& left, const bigint& right) {
     //left < right
     bool same_sign = (left.isnegative == right.isnegative);
     if(left.num.size() == right.num.size()){
       if(!same_sign && left.isnegative) return true;
       if(!same_sign && right.isnegative) return false;
       return abs_less(left,right); 
     }
     bool bigger = (left.num.size() < right.num.size()) ;
     if(!same_sign){return (left.isnegative);}
     if(!right.isnegative){
         return bigger;
      }else{
          return !bigger;
     }


}

bigint& bigint::operator= (const bigint& that){
      if(this == &that) return *this;
      isnegative = that.isnegative;
      long_value = that.long_value;
      num.clear();
      num = that.num;
      return *this;
}

ostream& operator<< (ostream& out, const bigint& that) {
   auto ritor = that.num.rbegin();
   int dig_limit = 19;
   if(ritor == that.num.rend() ) {out << "0";return out;}
   if(that.isnegative){ 
       out << "-";
    }
    int count {};
   for(;ritor != that.num.rend(); ritor++){
           out << static_cast<unsigned>(*ritor);
           if(++count ==dig_limit){
            cout<< "\\" << endl;
            count = 0;
           }
   }   
   return out;
}


bigint pow (const bigint& base, const bigint& exponent) {
   DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
   if (base == 0) return 0;
      bigint nl (numeric_limits<long>::max());
   if(nl < exponent){
    cout << "exponent is too large" << endl;
   }
   bigint base_copy = base;
   bigint zero(0);
   bigint result(1); bigint two (2);
   bigint one (1); bigint neg_one(-1);
   bigint expt = exponent;
   if (expt < zero) {
      base_copy = 1 / base_copy;
      expt = expt * neg_one ;
   }
   while ( zero < expt ){
      if (expt % two == one) { //odd
         result = result * base_copy;
         expt = expt - one; 
      }else { //even
         base_copy = base_copy*base_copy;
         divide_by_2(expt);
      }
   }
   DEBUGF ('^', "result = " << result);
   return result;
}
