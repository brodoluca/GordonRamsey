//
//  main.cpp
//  Json Parser
//
//  Created by Luca on 14/05/21.
//

#include <iostream>

#include "Json.hpp"
int main(int argc, const char * argv[]) {
    // insert code here...
    
    const auto Json = Json::Json::FromString("null");
    const auto JsonTrue = Json::Json::FromString("true");
    const auto JsonFalse = Json::Json::FromString("false");
    Json::Json a(nullptr);
    Json::Json b(true);
    Json::Json c(false);
    if ("null" == a.toString()) {
        std::cout << "To String null passed!\n";
    }
    if ("true" == b.toString()) {
        std::cout << "To String true passed!\n";
    }
    if ("false" == c.toString()) {
        std::cout << "To String false passed!\n";
    }

    if (Json == nullptr) {
        std::cout << "From String nullptr passed!\n";
    }
    if (JsonTrue == "true") {
        std::cout << "From String true passed!\n";
    }
    if ((JsonFalse == "false")==false) {
        std::cout << "From String false passed!\n";
    }
    
    std::cout << "\n";
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    
    const auto JsonCString = Json::Json::FromString("\"Hello World!\"");
    if (JsonCString == "Hello World!" ) {
        std::cout << "From String to C string passed!\n";
    }

    
    Json::Json d("Hello World!");
    if ("\"Hello World!\"" == d.toString()) {
        std::cout << "To C String from string passed!\n";
    }
    
    
    const auto JsonCppString = Json::Json::FromString(std::string("\"Hello World!\""));
    if (JsonCppString == std::string("Hello World!") ) {
        std::cout << "From String to C++ string passed!\n";
    }
    
    Json::Json e(std::string("Hello World!"));
    if ("\"Hello World!\"" == e.toString()) {
        std::cout << "To C++ String from c++ string passed!\n";
    }
    
    
    std::cout << "\n";
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    Json::Json f(std::string("Needs to be escaped, \", \\, \b, \f,\n,\r,\t "));
    if ("\"Needs to be escaped, \\\", \\\\, \\\b, \\\f,\\\n,\\\r,\\\t \"" == f.toString()) {
        std::cout << "Properly escaped string passed\n";
    }
    const auto JsonUnEscapedString = Json::Json::FromString(std::string("\"Needs to be unescaped,\\\", \\\\,\\\b,\\\f,\\\n,\\\r,\\\t \""));
    if (JsonUnEscapedString == std::string("Needs to be unescaped,\", \\,\b,\f,\n,\r,\t ") ) {
        std::cout << "Properly unescaped string passed\n";
    }
    std::cout << "\n";
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    
    Json::Json g(nullptr);
    if ((bool)g == false) {
        std::cout << "Bool down cast wrong value passed\n";
    }
    Json::Json h(false);
    if ((bool)h == false) {
        std::cout << "Bool down cast false passed\n";
    }
    
    Json::Json i(true);
    if ((bool)h == false) {
        std::cout << "Bool down cast false not passed\n";
    }else if ((bool)h == true){
        std::cout << "Bool down cast true passed\n";
    }
    
    std::cout << "\n";
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    Json::Json j(nullptr);
    if ((std::string)j == "") {
        std::cout << "String down cast wrong value passed\n";
    }
    Json::Json k("");
    if ((std::string)k == "") {
        std::cout << "String down cast empty string value passed\n";
    }
    Json::Json l("asdj");
    if ((std::string)l == "asdj") {
        std::cout << "String down cast string value passed\n";
    }
    
    std::cout << "\n";
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    Json::Json m(42.0);
    if (m.toString() == "42") {
        std::cout << "Integer test passed\n";
    }
    const auto n = Json::Json::FromString(std::string("42"));
    if (n == 42 ) {
        std::cout << "Integer passed\n";
    }
    
    Json::Json o(42.32);
    if (m.toString() == "42.32") {
        std::cout << "FLoat test passed\n";
    }
    
    const auto v = Json::Json::FromString(std::string("234.5"));
    if (v == 234.5 ) {
        std::cout << "float passed\n";
    }
    return 0;
}
