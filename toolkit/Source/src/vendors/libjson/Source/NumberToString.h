#ifndef NUMBERTOSTRING_H
#define NUMBERTOSTRING_H

#include "JSONDebug.h"
#include "JSONMemory.h"
#include <cstdio>

template <unsigned int GETLENSIZE>
struct getLenSize{ 
    char tmp[GETLENSIZE == 16];  // compile time assertion
    enum {GETLEN = 41};
};

template<>
struct getLenSize<1>{
    enum {GETLEN = 5};
};

template <>
struct getLenSize<2>{
    enum {GETLEN = 7};
};

template <>
struct getLenSize<4>{
    enum {GETLEN = 12};
};

template <>
struct getLenSize<8>{
    enum {GETLEN = 22};
};

class NumberToString {
public:
    template<typename T>
    static json_string _itoa(T val){
	   json_char result[getLenSize<sizeof(T)>::GETLEN];
	   result[getLenSize<sizeof(T)>::GETLEN - 1] = JSON_TEXT('\0'); //null terminator
	   json_char * runner = &result[getLenSize<sizeof(T)>::GETLEN - 2];
	   bool negative;
	   
	   START_MEM_SCOPE
		  long value = (long)val;
		  //first thing, check if it's negative, if so, make it positive
		  if (value < 0){
			 value = -value;
			 negative = true;
		  } else {
			 negative = false;
		  }
		  
		  //create the string
		  do {
			 *runner-- = (json_char)(value % 10) + JSON_TEXT('0');
		  } while(value /= 10);
	   END_MEM_SCOPE
	   
	   //if it's negative, add the negation
	   json_string res;
	   if (negative){
		  *runner = JSON_TEXT('-');
		  res = runner;
	   } else {
		  res = runner + 1;
	   }
	   return res;   
    }
    
    #ifndef JSON_LIBRARY
	   template<typename T>
	   static json_string _uitoa(T val){
		  json_char result[getLenSize<sizeof(T)>::GETLEN];
		  result[getLenSize<sizeof(T)>::GETLEN - 1] = JSON_TEXT('\0'); //null terminator
		  json_char * runner = &result[getLenSize<sizeof(T)>::GETLEN - 2];		  
		  
		  //create the string
		  START_MEM_SCOPE
			 unsigned long value = (unsigned long)val;
			 do {
				*runner-- = (json_char)(value % 10) + JSON_TEXT('0');
			 } while(value /= 10);
		  END_MEM_SCOPE
		  
		  json_string res = runner + 1;
		  return res;   
	   }
    #endif
    
    template<typename T>
    static json_string _ftoa(T value){
	   json_char result[64];
	   #ifdef JSON_UNICODE
		  swprintf(result, 63, L"%f", value); 
	   #else
		  //Thanks to Salvor Hardin for this Visual C++ fix
		  #if _MSC_VER
			 _snprintf_s(result, 63, 63, "%f", value); //yes, 63 appears twice using _snprintf_s()
		  #else
			 snprintf(result, 63, "%f", value); 
		  #endif
	   #endif
	   //strip the trailing zeros
	   for(json_char * pos = &result[0]; *pos; ++pos){
		  if (*pos == '.'){  //only care about after the decimal
			 for(json_char * runner = pos + 1; *runner; ++runner){
				if (*runner != JSON_TEXT('0')) pos = runner + 1;  //have to go to the end 1.0001
			 }
			 *pos = JSON_TEXT('\0');
			 break;
		  }
	   }
	   return result;    
    }
    
    static inline bool areEqual(const json_number & one, const json_number & two){
	   const json_number temp = one - two;
	   return (temp > 0.0) ? temp < 0.00001 : temp > -0.00001;   
    }
};

#endif
