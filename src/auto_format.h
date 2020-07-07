
#ifndef AUTOHOME_FORMAT_H
#define AUTOHOME_FORMAT_H

#include <Arduino.h>

//---------------- STRING_TYPE ------------------//
#define USE_SERIAL Serial
#define USE_STRING 1
#define USE_CHAR_P 2
#define USE_CONST_CHAR_P 3
//------------- STRING_TYPE Choice --------------//
#define USE_STRING_TYPE USE_CONST_CHAR_P
//-----------------------------------------------//
#if USE_STRING_TYPE == USE_STRING
#define STRING_TYPE String
#elif USE_STRING_TYPE == USE_CHAR_P
#define STRING_TYPE char*
#elif USE_STRING_TYPE == USE_CONST_CHAR_P
#define STRING_TYPE const char*
#endif
//-----------------------------------------------//

//----------- JSON_VALUE_STRING_TYPE ------------//
//--------JSON_VALUE_STRING_TYPE Choice ---------//
#define USE_JSON_VALUE_STRING_TYPE USE_CONST_CHAR_P
//-----------------------------------------------//
#if USE_JSON_VALUE_STRING_TYPE == USE_STRING
#define JSON_VALUE_STRING_TYPE String
#elif USE_JSON_VALUE_STRING_TYPE == USE_CHAR_P
#define JSON_VALUE_STRING_TYPE char*
#elif USE_JSON_VALUE_STRING_TYPE == USE_CONST_CHAR_P
#define JSON_VALUE_STRING_TYPE const char*
#endif
//-----------------------------------------------//


class AutoFormatClass {
#if USE_STRING_TYPE == USE_STRING
        //No implementation yet
#elif USE_STRING_TYPE == USE_CHAR_P
        //No implementation yet
#elif USE_STRING_TYPE == USE_CONST_CHAR_P
    private:
        static char response[1000];
        STRING_TYPE resp_p = response;
        static char second_str[500];
        STRING_TYPE sec_str_p = second_str;
    //protected:
    public:
    //------------- Number-to-String Conversion Functions --------------//
        void n2c(float number, char* result);
        void n2c(int number, char* result);
        void n2c(unsigned long number, char* result, size_t size_of_result);
        void n2c(signed long number, char* result, size_t size_of_result);
    //------------------------------------------------------------------//
    //------------- String-to-String Conversion Functions --------------//
        void s2c(String str, char* result);
        String cc2s(STRING_TYPE str);
    //------------------------------------------------------------------//
    //---------------- String-append-String Functions ------------------//
        STRING_TYPE ss(STRING_TYPE str1, STRING_TYPE str2);
        STRING_TYPE ss(STRING_TYPE str1, char* str2);
        STRING_TYPE ss(STRING_TYPE str1, uint8_t* str2);
        STRING_TYPE ss(STRING_TYPE str1, String str2);
    //------------------------------------------------------------------//
    //----------------- String-append-Number Functions -----------------//
        STRING_TYPE ss(STRING_TYPE str, float num);
        STRING_TYPE ss(STRING_TYPE str, int num);
        STRING_TYPE ss(STRING_TYPE str, unsigned int num);
        STRING_TYPE ss(STRING_TYPE str, unsigned long num);
        STRING_TYPE ss(STRING_TYPE str, signed long num);
        STRING_TYPE snua(STRING_TYPE phrase, float number, STRING_TYPE unit, STRING_TYPE union_article = " em ");
    //------------------------------------------------------------------//
    //----------------- String-Verification Functions ------------------//
        boolean sEqu(STRING_TYPE str1, STRING_TYPE str2);
        boolean sNul(STRING_TYPE str);
        size_t sLen(STRING_TYPE str);
        size_t sLen(char* str);
        size_t sLen(uint8_t* str);
        size_t sLen(String str);
    //------------------------------------------------------------------//
    //------------- String-to-Number Conversion Functions --------------//
        unsigned long s2ul(STRING_TYPE input);
        float s2f(STRING_TYPE input);
        int s2i(STRING_TYPE str);
    //------------------------------------------------------------------//
    //------------------ Same-Type Format Functions --------------------//
        float fFormat(float number, int precision = 2);
    //------------------------------------------------------------------//
#endif

#if USE_JSON_VALUE_STRING_TYPE == USE_STRING_TYPE
        //Nothing needs to be done
#else
    #if USE_JSON_VALUE_STRING_TYPE == USE_STRING
        //No implementation yet
    #elif USE_JSON_VALUE_STRING_TYPE == USE_CHAR_P
        //No implementation yet
    #elif USE_JSON_VALUE_STRING_TYPE == USE_CONST_CHAR_P
        //No implementation yet
    #endif
#endif
};

extern AutoFormatClass Format;

#endif
