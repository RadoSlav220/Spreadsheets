#include "../headers/cell.h"
#include <iostream>


//***************************intCell***************************//

intCell::intCell(int value) : value(value)
{
    s_value = std::to_string(value);
}

Type intCell::getType() { return Type::INT; }

int intCell::getValue() const { return value; }

std::string intCell::getS_Value () const { return s_value; }

double intCell::getNum_Value() { return double(value);}

void intCell::print() const { std::cout << value; }

size_t intCell::getSpacing()  
{
    return s_value.size();
}




//***************************doubleCell***************************//

doubleCell::doubleCell(double value) : value(value) 
{
    s_value = std::to_string(value);
    //Removing unnecessary '0'-s
    while (s_value.back() == '0'){
        s_value.pop_back();
    }

    if (s_value.back() == '.'){
        s_value.pop_back();
    }
}

Type doubleCell::getType() { return Type::DOUBLE; }

double doubleCell::getValue() const { return value; }

std::string doubleCell::getS_Value() const { return s_value; }

double doubleCell::getNum_Value() { return value;}

void doubleCell::print() const { std::cout << s_value; }

size_t doubleCell::getSpacing()  
{
    return s_value.size();
}




//***************************stringCell***************************//

stringCell::stringCell(const std::string& s_value) : s_value(s_value)
{
    /* OLD CODE
    value = s_value;
    
    //getting rid of the surrounding quotations
    value.erase(value.begin());
    value.pop_back();

    for (size_t i=0; i<value.size(); ++i){
        if (value[i] == '\\'){ //i+1 < realValue.size for sure
            value.erase(value.begin() + i);
            ++i;
        }
    }
    */

    //--------------------------------------------//
    //Avoiding the surrounding quotations
    for (size_t i=1; i<s_value.size()-1; ++i){
        if (s_value[i] == '\\'){
            ++i;
        }
        value.push_back(s_value[i]);
    }
    //--------------------------------------------//
}

Type stringCell::getType() { return Type::STRING; }

const std::string& stringCell::getValue() const { return value; }

std::string stringCell::getS_Value() const { return s_value; }

double stringCell::getNum_Value()
{
    try {
        return std::stod(value);
    } catch (std::exception& e){
        return 0.0;
    }
}

void stringCell::print() const { std::cout << value; }

size_t stringCell::getSpacing() { return value.size(); }



//***************************emptyCell***************************//

Type emptyCell::getType() { return Type::EMPTY; }

void emptyCell::print() const {}

size_t emptyCell::getSpacing() { return 0; }

std::string emptyCell::getS_Value() const { return std::string(); }

double emptyCell::getNum_Value() { return 0.0;}