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
//
    if (Json == nullptr) {
        std::cout << "From String nullptr passed!\n";
    }
    if (JsonTrue == "true") {
        std::cout << "From String true passed!\n";
    }
    if (JsonFalse == "false") {
        std::cout << "From String false passed!\n";
    }
    return 0;
}
