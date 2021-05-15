//
//  Json.hpp
//  Json Parser
//
//  Created by Luca on 14/05/21.
//

#ifndef Json_hpp
#define Json_hpp

#include <stdio.h>

#include <memory>
#include <string>
#include <set>
#include <iostream>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
///TO DO - UNICODE I cant be bothered


namespace{
///characters that must be escaped
const std::set<char> CHARACTERS_TO_ESCAPE_IN_QUOTED_STRING{
    '"',///quotation mark  0x22
    '\\',///0x5c
    '\b', ///backspace \x08
    '\f',///0x0c
    '\n',///0x0a
    '\r',///0x0d
    '\t',///\x09 = horizonal tab
};

///this is used to define the states when parsing integers or float
enum class numberState{
    minusSign,
    zeroOrDigit,
    extraJunk,
    digit,
    fraction,
    exponentSignOrDigit,
    fractionOptional,
    exponentDigit,
    exponentExtraDigits
};


};

namespace Json{

///used to configure various oprionshaving to do with encoding json object into a string format
struct EncodingOptions{
    ///defines wether we escape non esci chars or not
    bool escapeNonAscii = false;
};

class Json{
public:
    ~Json();
    Json(const Json&) = delete;
    Json(Json&&);
    Json& operator = (const Json&) = delete;
    Json& operator = (Json&&);
    
    Json();
    ///constructs a json object consisting of a literal "null" (check Json format)
    ///@param null
    /// object to wrap in Json
    Json(nullptr_t null);
    
    ///constructs a json object consisting of a boolean value (check Json format)
    ///@param value
    /// object to wrap in Json
    Json(bool value);
    
    ///constructs a json object consisting of an integer
    ///@param value
    /// object to wrap in Json
    Json(int value);
    
    ///constructs a json object consisting of a floating point
    ///@param value
    /// object to wrap in Json
    Json(double value);
    
    ///constructs a json object consisting of a c string value (check Json format)
    ///@param value
    /// object to wrap in Json
    Json(const char* value);
    
    ///constructs a json object consisting of a c++ string value (check Json format)
    ///@param value
    /// object to wrap in Json
    Json(const std::string& value);
    
    ///equality comparison operator
    ///@param other
    /// other object to compare it to
    ///@return
    /// true if equal, false if not
    /// equal but not identical
    bool operator == (const Json& other) const;
    
    ///equality comparison operator between this object and int
    ///@param other
    /// other int to compare it to
    ///@return
    /// true if equal, false if not
    /// equal but not identical
    bool operator == (int other) const;
    
    ///equality comparison operator between this object and double
    ///@param other
    /// other double to compare it to
    ///@return
    /// true if equal, false if not
    /// equal but not identical
    bool operator == (double other) const;
    
    ///downcasts the object as a bool
    ///@retval
    ///true is returned if the object is a bool and true
    ///@retval
    ///false is returned if the object IS NOT a bool or if the value is false
    operator bool() const;
    
    ///downcasts the object as a integer
    ///@retval
    ///The value of the integer
    ///@retval
    ///0 if it's not a zeor of it's an integer of value 0
    operator int() const;
    
    ///downcasts the object as a floating point
    ///@retval
    ///The value of the double
    ///@retval
    ///0.0 if it's not a number
    operator double() const;
    
    ///downcasts the object as a string Cpp
    ///@retval ""
    ///returned if it's not a string or its value is an empty string
    ///@retval
    ///returns the value of the string
    operator std::string() const;
    
    ///encodes the Json object into a stirng format
    ///@param
    ///option -used to configure various oprionshaving to do with encoding json object into a string format
    ///@return
    ///the string format of the Json object
    std::string toString(const EncodingOptions& option = EncodingOptions()) const;
    
    ///returns a Json object constructed from the given format
    ///@param format
    /// plain string cintaining the Json
    ///@return
    /// the Json object fromby parsing the json formato from the given string
    static Json FromString(const std::string& format );
    
    
private:
    
    ///we declare it here so private proprierties are not visible.
    ///Struct declared here limits it to the scope of this class
    struct impl;
    std::unique_ptr< struct impl > impl_;
    
    ///helper function to cast float to string and remove zeros at the end
    std::string floatToString(double value)const;
};




};

///this function produces the escaped versione of the given string. The char and set can be also set as standard because Json sets them as default, but I like it better this way
///@param
///escapeString - string to be escaped
///escapeChar - char to include before the char that needs to be scaped
///escapeSet - set of char that must be considered to be escaped
///option -used to configure various oprionshaving to do with encoding json object into a string format
///@return
///escaped string
std::string escape(std::string escapeString, char escapeChar, const std::set<char>& escapeSet, const Json::EncodingOptions& option);

///this function produces the unescaped versione of the given string. The char can be also set as standard because Json sets them as default, but I like it better this way
///@param
///unescapeString - string to be unescaped
///escapeChar - char to remove
///@return
///unescaped string
std::string unescape(std::string unescapeString, char escapeChar);

///this function parses an integer from a given string
///@param
///s string that needs to be parsed
///@return
///Json object related to that integer
Json::Json parseInteger(const std::string& s);

///this function parses a float from a given string
///@param
///s string that needs to be parsed
///@return
///Json object related to that float
Json::Json parseFloat(const std::string& s);
#endif /* Json_hpp */
