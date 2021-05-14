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
};








namespace Json{

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
    
    
    
    ///downcasts the object as a bool
    ///@retval
    ///true is returned if the object is a bool and true
    ///@retval
    ///false is returned if the object IS NOT a bool or if the value is false
    operator bool() const;
    
    ///downcasts the object as a string Cpp
    ///@retval ""
    ///returned if it's not a string or its value is an empty string
    ///@retval
    ///returns the value of the string
    operator std::string() const;
    
    ///encodes the Json object into a stirng format
    ///@return
    ///the string format of the Json object
    std::string toString() const;
    
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
};




};




inline std::string escape(std::string escapeString, char escapeChar, const std::set<char>& escapeSet){
    
    std::set<int> temp;
    for (std::set<char>::iterator i = escapeSet.begin(); i != escapeSet.end(); i++) {
        temp.insert(*i);
    }
    for (int i= 0; i< escapeString.size(); i++) {
        int temp_int = int(escapeString[i]);
        if (temp.find(temp_int) != temp.end()) {
            escapeString.insert(escapeString.begin()+i, escapeChar);
            i++;
        }
    }
    return escapeString;
}

inline std::string unescape(std::string escapeString, char escapeChar){
    
    for (auto i = escapeString.begin(); i != escapeString.end(); i++) {
        if (*i == escapeChar) {
            escapeString.erase(i);
        }
    }
   
    
    return escapeString;
}

#endif /* Json_hpp */
