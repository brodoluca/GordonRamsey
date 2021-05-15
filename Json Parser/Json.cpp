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
        String,
        Integer,
        FloatingPoint
        
    };
    
    ///type of the Json object
    type type_ = type::Invalid;
    
    ///this holds the actual value of the Json object
    ///use the member that matches the type
    union{
        ///in case it's a boolean, we want to remember if it's true or false
        bool booleanValue;
        std::string* stringValue;
        int integerValue;
        double floatingPointValue;
    };
    
    
    ///members
    
    
    ///custom constructor
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

Json::Json(int value):impl_(new impl){
    impl_->type_ = impl::type::Integer;
    impl_->integerValue = value;
}

Json::Json(double value):impl_(new impl){
    impl_->type_ = impl::type::FloatingPoint;
    impl_->floatingPointValue = value;
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
            case impl::type::Integer:
                return impl_->integerValue == other.impl_->integerValue;
                break;
            case impl::type::FloatingPoint:
                return impl_->floatingPointValue == other.impl_->floatingPointValue;
                break;
            default:
                ///invalids are always equal
                return true;
                break;
        }
        
    }
    return true;
}

bool Json::operator == (int other) const{
    return impl_->integerValue == other;
}

bool Json::operator == (double other) const{
    return impl_->floatingPointValue == other;
}

Json::operator bool() const{
    if(impl_->type_ != impl::type::Boolean) return false;
    return impl_->booleanValue;
}
Json::operator int() const{
    if(impl_->type_ != impl::type::Integer) return 0;
    else if (impl_->type_ != impl::type::FloatingPoint) return (int) impl_->floatingPointValue;
    return impl_->integerValue;
}

Json::operator double() const{
    if(impl_->type_ == impl::type::Integer) return (double)impl_->integerValue;
    else if (impl_->type_ == impl::type::FloatingPoint) return impl_->floatingPointValue;
    return 0.0;
}


Json::operator std::string() const{
    if(impl_->type_ != impl::type::String) return "";
    return *impl_->stringValue;
}

std::string Json::toString(const EncodingOptions& option) const{
    switch (impl_->type_) {
        case impl::type::Null:
            return "null";
            break;
        case impl::type::Boolean:
            return impl_->booleanValue? "true" : "false";
            break;
        case impl::type::String:
            return ("\""+ escape(*impl_->stringValue, '\\', CHARACTERS_TO_ESCAPE_IN_QUOTED_STRING, option) +"\"");
            break;
        case impl::type::Integer:
            return std::to_string(impl_->integerValue);
        case impl::type::FloatingPoint:
            return floatToString(impl_->floatingPointValue);
        default:
            return "???";
            break;
    }
}


Json Json::FromString(const std::string& format ){
    if (format.empty()) {
        return Json();
    }else if (format == "null") {
        return nullptr;
    }else if (format == "true"){
        return true;
    }else if (format == "false"){
        return false;
    }else if (!format.empty() && format[0] == '"' && format[format.size()-1] == '"'){
        return unescape(format.substr(1, format.size()-2), '\\');
    }else if (!format.empty() && format[0] == '{'){
        return Json(); ///@todo Pars objects
    }else if (!format.empty() && format[0] == '['){
        return Json(); ///@todo Pars arrays
    }else if(format.find_first_of("+.eE") != std::string::npos) {
        return parseFloat(format); ///@todo parse as floating
    }else {
        return parseInteger(format); 
        
    }
    
}

Json::Json():impl_(new impl){

    
}


std::string Json::floatToString(double value)const{
    std::string output;
    output = std::to_string(value);
    while (output.back() == '0') output.pop_back();
    return output;
}





}



std::string escape(std::string escapeString, char escapeChar, const std::set<char>& escapeSet, const Json::EncodingOptions& option){
    std::string output;
    for (int i= 0; i< escapeString.size(); i++) {
        if (escapeSet.find(escapeString[i]) != escapeSet.end()) {
            output += escapeChar;
            ///this is used to to the non Ascii stuff (UNICODE), but I dont care
        }else if(option.escapeNonAscii && (escapeString[i] < 0x20 || escapeString[i] == '=' || escapeString[i]=='\\')){
            output += "\\u";
            
        }
        output += escapeString[i];
    }
    return output;
}


std::string unescape(std::string unescapeString, char escapeChar){
    std::string output;
    bool escape = false;
    for (int i= 0; i< unescapeString.size(); ++i) {
        if (!escape && (unescapeString[i] == escapeChar)) {
            escape = true;
        }else{
            output += unescapeString[i];
            escape = false;
        }
    }
    return output;
}


Json::Json parseInteger(const std::string& s){
    size_t index = 0;
    numberState state = numberState::minusSign;
    bool negative = false;
    int outputValue = 0;
    while (index < s.length()) {
        switch (state) {
                ///[minus]
            case numberState::minusSign:
                if (s[index] == '-') {
                    negative = true;
                    ++index;
                }
                state = numberState::zeroOrDigit;
                break;
                
                ///[digits either 0 or 1 - 9]
            case numberState::zeroOrDigit:
                if (s[index] == '0') {
                    state = numberState::extraJunk;
                }else if(s[index] >= '1' &&s[index] <= '9'){
                    state = numberState::digit;
                    outputValue = (int)(s[index] - '0');
                }else{
                    return Json::Json();
                }
                ++index;
                break;
                
                ///extraJunk
            case numberState::extraJunk:
                return Json::Json(); ///return invalid object
                break;
                
                ///digit
            case numberState::digit:
                if(s[index] >= '0' &&s[index] <= '9'){
                    const int previousValue = outputValue;
                    outputValue*=10;
                    outputValue += (int)(s[index] - '0');
                    if(outputValue /10 != previousValue ) return  Json::Json();;
                }else{
                    return Json::Json();
                }
                ++index;
                break;
                
            default:
                break;
        }
    }
    
    ///if we are still in this state, the parses is trying to parse only junk
    ///Therefore, we return invalid
    if(state == numberState::minusSign)
        return Json::Json();
    
    if(negative == true)
        return Json::Json(0 - outputValue);
    
    return Json::Json(outputValue);
    
}



Json::Json parseFloat(const std::string& s){
    size_t index = 0;
    numberState state = numberState::minusSign;
    bool negative = false;
    bool negativeExponent = false;
    double magnitude = 0;
    double fraction = 0.0;
    double exponent = 0.0;
    size_t fractionDigit = 0;
    while (index < s.length()) {
        switch (state) {
                ///[minus]
            case numberState::minusSign:
                if (s[index] == '-') {
                    negative = true;
                    ++index;
                }
                state = numberState::zeroOrDigit;
                break;
                
                ///[digits either 0 or 1 - 9]
            case numberState::zeroOrDigit:
                if (s[index] == '0') {
                    state = numberState::extraJunk;
                }else if(s[index] >= '1' &&s[index] <= '9'){
                    state = numberState::digit;
                    magnitude = (double)(s[index] - '0');
                }else{
                    return Json::Json();
                }
                ++index;
                break;
                
                ///extraJunk
            case numberState::extraJunk:
                return Json::Json(); ///return invalid object
                break;
                
                ///digit or decimal point
            case numberState::digit:
                if(s[index] >= '0' &&s[index] <= '9'){
                    magnitude*=10.0;
                    magnitude += (double)(s[index] - '0');
                }else if (s[index] == '.'){
                    state = numberState::fraction;
                }else if (s[index] == 'e' || s[index] == 'E') {
                    state = numberState::exponentSignOrDigit;
                }
                else{
                    return Json::Json();
                }
                ++index;
                break;
                
            case numberState::fraction:
                if(s[index] >= '0' &&s[index] <= '9'){
                    ++fractionDigit;
                    fraction += (double)(s[index] - '0') / pow(10.0, (double)fractionDigit);
                }else{
                    return Json::Json();
                }
                state = numberState::fractionOptional;
                ++index;
                break;
                
            case numberState::fractionOptional:
                if(s[index] >= '0' &&s[index] <= '9'){
                    ++fractionDigit;
                    fraction += (double)(s[index] - '0') / pow(10.0, (double)fractionDigit);
                }else if (s[index] == 'e' || s[index] == 'E') {
                    state = numberState::exponentSignOrDigit;
                }else{
                    return Json::Json();
                }
                ++index;
//                state = numberState::exponentSignOrDigit;
                break;
               
                ///exponent [minus/plus] /  DIGIT
            case numberState::exponentSignOrDigit:
                if(s[index] == '-'){
                    negativeExponent = true;
                    ++index;
                }else if(s[index] == '+'){
                    ++index;
                }
                state = numberState::exponentDigit;
                break;
                
            case numberState::exponentDigit:
                state = numberState::exponentExtraDigits;
                break;
                
                
            case numberState::exponentExtraDigits:
                if(s[index] >= '0' &&s[index] <= '9'){
                    exponent*=10.0;
                    exponent += (double)(s[index] - '0');
                }else{
                    return Json::Json();
                }
                ++index;
                break;
            default:
                break;
        }
    }
    
    ///if we are still in this state, the parses is trying to parse only junk
    ///Therefore, we return invalid
    if(state == numberState::minusSign ||state == numberState::fraction ||state == numberState::exponentSignOrDigit ||state == numberState::exponentDigit  )
        return Json::Json();
    
    
    return Json::Json((magnitude + fraction)* pow(10.0, exponent*(negativeExponent ? -1.0 : 1.0 ))*(negative ? -1.0 : 1.0 ));
                      
                    
}
