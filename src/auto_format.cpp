#include "auto_format.h"


#if USE_STRING_TYPE == USE_STRING
//No implementation yet
#elif USE_STRING_TYPE == USE_CHAR_P
//No implementation
#elif USE_STRING_TYPE == USE_CONST_CHAR_P
//------------- Number-to-String Conversion Functions --------------//
//void n2c(float number, char* result);
void AutoFormatClass::n2c(float number, char* result) {
  dtostrf(number, 5,2, result);
}
//void n2c(int number, char* result);
void AutoFormatClass::n2c(int number, char* result) {
  itoa(number, result, 10);
}
//void n2c(unsigned long number, char* result, size_t size_of_result);
void AutoFormatClass::n2c(unsigned long number, char* result, size_t size_of_result) {
  snprintf(result, size_of_result, "%ld", number);
}
//void n2c(signed long number, char* result, size_t size_of_result);
void AutoFormatClass::n2c(signed long number, char* result, size_t size_of_result) {
  snprintf(result, size_of_result, "%ld", number);
}
//------------------------------------------------------------------//
//------------- String-to-String Conversion Functions --------------//
//void s2c(String str, char* result);
void AutoFormatClass::s2c(String str, char* result) {
  str.toCharArray(result, str.length()+1);
}
//String cc2s(STRING_TYPE str);
String AutoFormatClass::cc2s(STRING_TYPE str) { //https://stackoverflow.com/questions/54276088/join-const-char-in-a-string
  return String() + str;
}
//------------------------------------------------------------------//
//---------------- String-append-String Functions ------------------//
//STRING_TYPE ss(STRING_TYPE str1, STRING_TYPE str2);
STRING_TYPE AutoFormatClass::ss(STRING_TYPE str1, STRING_TYPE str2) {
  if(resp_p == str2) {
    strcpy(second_str,str2);
    strcpy(response,str1);
    strcat(response,sec_str_p);
  } else {
    strcpy(response,str1);
    strcat(response,str2);
  }
  return (STRING_TYPE)resp_p;
}
//STRING_TYPE ss(STRING_TYPE str1, char* str2);
STRING_TYPE AutoFormatClass::ss(STRING_TYPE str1, char* str2) {
  return ss(str1, (STRING_TYPE)str2);
}
//STRING_TYPE ss(STRING_TYPE str1, uint8_t* str2);
STRING_TYPE AutoFormatClass::ss(STRING_TYPE str1, uint8_t* str2) {
  return ss(str1, (STRING_TYPE)str2);
}
//STRING_TYPE ss(STRING_TYPE str1, String str2);
STRING_TYPE AutoFormatClass::ss(STRING_TYPE str1, String str2) {
  char str_2[str2.length()+1];
  size_t l = sLen(str1) + sLen(str_2);
  if(l > strlen(response)) return "";
  s2c(str2, str_2);
  return ss(str1, str_2);
}
//------------------------------------------------------------------//
//----------------- String-append-Number Functions -----------------//
//STRING_TYPE ss(STRING_TYPE str, float num);
STRING_TYPE AutoFormatClass::ss(STRING_TYPE str, float num) {
  char num_c[18];
  n2c(num, num_c);
  return ss(str, num_c);
}
//STRING_TYPE ss(STRING_TYPE str, int num);
STRING_TYPE AutoFormatClass::ss(STRING_TYPE str, int num) { //int16_t is not long, generally int
  char num_c[18];
  n2c(num, num_c);
  return ss(str, num_c);
}
//STRING_TYPE ss(STRING_TYPE str, unsigned int num);
STRING_TYPE AutoFormatClass::ss(STRING_TYPE str, unsigned int num) {
  return ss(str,(int)num);
}
//STRING_TYPE ss(STRING_TYPE str, unsigned long num);
STRING_TYPE AutoFormatClass::ss(STRING_TYPE str, unsigned long num) { //uint32_t is generally unsigned long
  char num_c[36];
  n2c(num, num_c, sizeof(num_c));
  return ss(str, num_c);
}
//STRING_TYPE ss(STRING_TYPE str, signed long num);
STRING_TYPE AutoFormatClass::ss(STRING_TYPE str, signed long num) { //int32_t is generally unsigned long
  char num_c[36];
  n2c(num, num_c, sizeof(num_c));
  return ss(str, num_c);
}
//STRING_TYPE snua(STRING_TYPE phrase, float number, STRING_TYPE unit, STRING_TYPE union_article = " em ");
STRING_TYPE AutoFormatClass::snua(STRING_TYPE phrase, float number, STRING_TYPE unit, STRING_TYPE union_article) {
  return ss(ss(ss(phrase, union_article),number),unit);
}
//------------------------------------------------------------------//
//----------------- String-Verification Functions ------------------//
//boolean sEqu(STRING_TYPE str1, STRING_TYPE str2);
boolean AutoFormatClass::sEqu(STRING_TYPE str1, STRING_TYPE str2) {
  return (boolean)!strcmp(str1, str2);
}
//boolean sNul(STRING_TYPE str);
boolean AutoFormatClass::sNul(STRING_TYPE str) {
  if(!str) return true;
  if(*str == '\0') return true;
  return false;
}
//size_t sLen(STRING_TYPE str);
size_t AutoFormatClass::sLen(STRING_TYPE str) {
  return strlen(str);
}
//size_t sLen(char* str);
size_t AutoFormatClass::sLen(char* str) {
  return sLen((STRING_TYPE)str);
}
//size_t sLen(uint8_t* str);
size_t AutoFormatClass::sLen(uint8_t* str) {
  return sLen((STRING_TYPE)str);
}
//size_t sLen(String str);
size_t sLen(String str) {
    return (size_t)str.length();
}
//------------------------------------------------------------------//
//------------- String-to-Number Conversion Functions --------------//
//unsigned long s2ul(STRING_TYPE input);
unsigned long AutoFormatClass::s2ul(STRING_TYPE input) {
  return strtoul(input, NULL, 0);
}
//float s2f(STRING_TYPE input);
float AutoFormatClass::s2f(STRING_TYPE input) {
  return strtof(input, NULL);
}
//int s2i(const char* str);
int AutoFormatClass::s2i(const char* str) {
  if(!sNul(str))
    return atoi(str);
  return 0;
}
//------------------------------------------------------------------//
//------------------ Same-Type Format Functions --------------------//
//float fFormat(float number, int precision = 2);
float AutoFormatClass::fFormat(float number, int precision) { 
    return floor(pow(10,precision)*number)/pow(10,precision); 
}
//----------------------------------------------------------------//
#endif

#if USE_JSON_VALUE_STRING_TYPE == USE_STRING_TYPE
        //Nothing needs to be done
#else
    #if USE_JSON_VALUE_STRING_TYPE == USE_STRING
        //No implementation yet
    #elif USE_JSON_VALUE_STRING_TYPE == USE_CHAR_P
        //No implementation
    #elif USE_JSON_VALUE_STRING_TYPE == USE_CONST_CHAR_P
        //No implementation
    #endif
#endif