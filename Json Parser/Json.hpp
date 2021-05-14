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
namespace Json{

class Json{
public:
    ~Json();
    Json(const Json&) = delete;
    Json(Json&&);
    Json& operator = (const Json&) = delete;
    Json& operator = (Json&&);
    
    
    
    
    ///equality comparison operator
    ///@param other
    /// other object to compare it to
    ///@return
    /// true if equal, false if not
    /// equal but not identical
    bool operator == (const Json& other) const;
    
    
    Json();
    ///constructs a json object consisting of a literal "null" (check Json format)
    ///@param null
    /// object to wrap in Json
    Json(nullptr_t null);
    
    ///constructs a json object consisting of a boolean value (check Json format)
    ///@param value
    /// object to wrap in Json
    Json(bool value);
    
    
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

#endif /* Json_hpp */
