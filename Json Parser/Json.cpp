//
//  Json.cpp
//  Json Parser
//
//  Created by Luca on 14/05/21.
//

#include "Json.hpp"


namespace Json {


///definition of the struct including the private proprierties
struct Json::impl{
    
    
    
    ///these are the types of value a Json object can be
    enum class type{
        Null,
        Boolean,
        Invalid,
        String
    };
    ///type of the Json object
    type type_ = type::Invalid;
    
    ///this holds the actual value of the Json object
    ///use the member that matches the type
    union{
        ///in case it's a boolean, we want to remember if it's true or false
        bool booleanValue;
        std::string* stringValue;
    };
    ///members
    //custom constructor
    ~impl(){
        switch (type_) {
            case impl::type::String:
                delete stringValue;
                break;
                
            default:
                break;
        }
    };
    impl(const impl&) = delete;
    impl(impl&&)= delete;
    impl& operator = (const impl&) = delete;
    impl& operator = (impl&&)= delete;

    ///default constructor
    impl()=default;
};
///default constructor will do
Json::~Json() = default;
Json::Json(Json&&) = default;
Json& Json::operator = (Json&&) = default;

Json::Json(nullptr_t null):impl_(new impl){
    impl_->type_ = impl::type::Null;
}

Json::Json(bool value):impl_(new impl){
    impl_->type_ = impl::type::Boolean;
    impl_->booleanValue = value;
}

Json::Json(const char* value):impl_(new impl){
    impl_->type_ = impl::type::String;
    impl_->stringValue = new std::string(value);
}


Json::Json(const std::string& value):impl_(new impl){
    impl_->type_ = impl::type::String;
    impl_->stringValue = new std::string(value);
}


bool Json::operator == (const Json& other) const{
    if (impl_->type_ != other.impl_->type_) {
        return false;
    }else{
        switch (impl_->type_) {
            case impl::type::Null:
                ///null is always equal
                return true;
                break;
            case impl::type::Boolean:
                ///true if they are actually equal
                return impl_->booleanValue == other.impl_->booleanValue;
                break;
            case impl::type::String:
                return *impl_->stringValue == *other.impl_->stringValue;
                break;
            default:
                ///invalids are always equal
                return true;
                break;
        }
        
    }
    return true;
}


Json::operator bool() const{
    if(impl_->type_ != impl::type::Boolean) return false;
    return impl_->booleanValue;
}
Json::operator std::string() const{
    if(impl_->type_ != impl::type::String) return "";
    return *impl_->stringValue;
}

std::string Json::toString() const{
    switch (impl_->type_) {
        case impl::type::Null:
            return "null";
            break;
        case impl::type::Boolean:
            return impl_->booleanValue? "true" : "false";
            break;
        case impl::type::String:
            return ("\""+ escape(*impl_->stringValue, '\\', CHARACTERS_TO_ESCAPE_IN_QUOTED_STRING) +"\"");
            break;
            
        default:
            return "???";
            break;
    }
    
}

Json Json::FromString(const std::string& format ){
    if (format == "null") {
        return nullptr;
    }else if (format == "true"){
        return true;
    }else if (format == "false"){
        return false;
    }else if (!format.empty() && format[0] == '"' && format[format.size()-1] == '"'){
        return unescape(format.substr(1, format.size()-2), '\\');
    }else{
        return Json();
    }
    
}

Json::Json():impl_(new impl){

    
}






}
