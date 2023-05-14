#include "../headers/formulaCell.h"
#include "../headers/table.h"
#include <iostream>
#include <queue>
#include <cmath>



formulaCell::formulaCell(const std::string& value, Table* ptr) : value (value), result(0), table(ptr)
{}

Type formulaCell::getType() { return Type::FORMULA; }

std::string formulaCell::getS_Value() const { return value; }

const std::vector<Cell**>& formulaCell::getDependingOn() { return dependingOn; }

//print() is always called after calling getSpacing()
//that means result_string is re-calculated every time print() is called
void formulaCell::print() const 
{ 
    std::cout << result_string; 
}

size_t formulaCell::getSpacing() 
{
    try {
        result = calculate();
    } catch (const std::logic_error& e){
        result_string = "#ERROR";
        return 6; //#ERROR
    }
    
    result_string = std::to_string(result);
    //delete unnecessary zeros
    while (result_string.back() == '0'){
        result_string.pop_back();
    }

    if (result_string.back() == '.'){
        result_string.pop_back();
    }
    return result_string.size();
}

double formulaCell::getNum_Value() 
{
    return calculate();
}


void formulaCell::fill_dependingOn()
{
    dependingOn.clear();
    
    for (size_t i=1; i<value.size(); ++i){ //to start after the '=' symbol
        if (value[i] >= 'A' && value[i] <= 'Z'){
            char col = value[i];
            ++i;
            std::string number;
            while (i<value.size() && !isOperator(value[i])){
                number.push_back(value[i]);
                ++i;
            }
            --i;
            size_t row = std::stoi(number);
            dependingOn.push_back(table->getCell(col, row));
        }
    }
    
    //removing nullptrs from dependingOn vector
    for (size_t i=0; i<dependingOn.size(); ++i){
        if (dependingOn[i] == nullptr){
            dependingOn.erase(dependingOn.begin() + i);
            --i;
        }
    }
    
}


double formulaCell::calculate()
{
    fill_dependingOn();
    std::vector <Cell*> forbiddenCells;
    forbiddenCells.push_back(this);
    checkForRecursion(this, forbiddenCells); //throws if there is recursion
    
    std::string calc_exp = getCalculationExpression(); //expression only with operands and numbers
    return shuntingYard(calc_exp);
}



std::string formulaCell::getCalculationExpression()
{
    std::string calc_exp;
    for (size_t i=1; i<value.size(); ++i){ //start from 1 to avoid = symbol

        if (isDigit(value[i]) || isOperator(value[i]) || value[i] == '.'){
            calc_exp.push_back(value[i]);
        }
        else {
            std::string cellAddress;
            while (i<value.size() && !isOperator(value[i])){
                cellAddress.push_back(value[i]);
                ++i;
            }
            --i;

            char col = cellAddress[0]; //get the column
            cellAddress.erase(0,1);
            Cell** found = table->getCell(col, std::stoi(cellAddress));
      
            if (!found){
                calc_exp += '0';
            }
            else {
                std::string num_value = std::to_string((*found)->getNum_Value());
                calc_exp += num_value;
            }
            
        }
    }
    return calc_exp;
}



bool formulaCell::isOperator(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}


bool formulaCell::isDigit (char c)
{
    return (c >= '0' && c <= '9');
}


bool formulaCell::isBiggerOperator(char c, char top)
{
    if ( ( (c == '*' || c == '/') && ( top == '-'  || top == '+' )) ||
         (c == '^' && ( top == '-' || top == '+' || top == '*' || top == '/')) )
         return true;

    return false;
}



double formulaCell::shuntingYard(const std::string& exp)
{
    std::queue <double> numbers;
    std::vector <char> operators;
    std::queue <char> queue;

    bool partOfNumber = false; //true if the last read char was an operator
    for (size_t i=0; i<exp.size(); ++i){
        if (isDigit(exp[i]) || partOfNumber){
            std::string number;
            number.push_back(exp[i]);
            ++i;
            while (i<exp.size() && !isOperator(exp[i])){
                number.push_back(exp[i]);
                ++i;
            }
            --i;
            numbers.push(stod(number));
            queue.push('#'); //means there is a number on this position
            partOfNumber = false;
        }

        else { //exp[i] is operator

            if (isOperator(exp[i-1])){
                partOfNumber = true;
                --i;
                continue;
            }

            while ( operators.size() != 0 && !isBiggerOperator(exp[i], operators.back()) ){
                char op = operators.back();
                queue.push(op);
                operators.pop_back();
            }
            operators.push_back(exp[i]);
        
        }
    }

    //adding the remaining operators to the queue
    while (!operators.empty()){
        queue.push(operators.back());
        operators.pop_back();
    }

    std::vector <double> number_stack;

    while (!queue.empty()){
        char cur = queue.front();
        if ( cur == '#' ){
            number_stack.push_back(numbers.front());
            numbers.pop();
        }
        else { //cur is operator
            double num2 = number_stack.back();
            number_stack.pop_back();
            double num1 = number_stack.back();
            number_stack.pop_back();

            double res; //the result
            switch (cur){
                case '+': res = num1 + num2; break;
                case '-': res = num1 - num2; break;
                case '*': res = num1 * num2; break;
                case '/': if ( abs(num2-0) < 0.00001 ) throw std::invalid_argument("Division by 0 is forbidden"); 
                          res = num1 / num2; break;
                case '^': res = pow (num1, num2); break;
                default: std::runtime_error("Unexpected error!");
            }
            number_stack.push_back(res);
        }
        queue.pop();
    }

    return number_stack.back();
}


void formulaCell::checkForRecursion(formulaCell* cell, std::vector<Cell*>& forbiddenCells)
{
    //to_check consist of the cells which the current cell depends on
    std::vector <Cell**> to_check = cell->getDependingOn();

    for (size_t i=0; i<to_check.size(); ++i){
        for (size_t j=0; j<forbiddenCells.size(); ++j){
            if (*to_check[i] == forbiddenCells[j]){
                throw std::logic_error("Recursion!");
            }
        }
    }

    
    for (size_t i=0; i<to_check.size(); ++i){
        if ( (*to_check[i])->getType() == Type::FORMULA ){
            forbiddenCells.push_back(cell);
            checkForRecursion(dynamic_cast<formulaCell*>(*to_check[i]), forbiddenCells);
            forbiddenCells.pop_back();
        }
    }
}