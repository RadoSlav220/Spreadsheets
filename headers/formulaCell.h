#pragma once
#include "cell.h"
#include <vector>


class Table;


//////////////////////////////////////////////////////
///@brief Cell with a value of type formula.
///
//////////////////////////////////////////////////////
class formulaCell : public Cell {

private:

    //////////////////////////////////////////////////////
    ///@brief The value of the formulaCell in form of a string expression.
    ///       May contain valid cell references.
    //////////////////////////////////////////////////////
    std::string value;

    //////////////////////////////////////////////////////
    ///@brief The result which calculating formula expression leads to.
    ///
    //////////////////////////////////////////////////////
    double result;

    //////////////////////////////////////////////////////
    ///@brief The result which calculating formula expression leads to in string form.
    ///
    //////////////////////////////////////////////////////
    std::string result_string;
    

    //////////////////////////////////////////////////////
    ///@brief The table which contains the current formuylaCell.
    ///
    //////////////////////////////////////////////////////
    Table* table;

    //////////////////////////////////////////////////////
    ///@brief Vector which contains Cell** pointers to all cells which the current formulaCell depends on.
    ///
    //////////////////////////////////////////////////////
    std::vector <Cell**> dependingOn; 

public:

    //////////////////////////////////////////////////////
    ///@brief Construct a new formula Cell object.
    ///
    ///@param value Valid calculating expression. May contain valid cell references.
    ///@param ptr Pointer to the table the current formulaCell is part of.
    //////////////////////////////////////////////////////
    formulaCell (const std::string& value, Table* ptr);


    //////////////////////////////////////////////////////
    ///@brief Get the Type of the formulaCell object.
    ///
    ///@return Type::FORMULA. 
    //////////////////////////////////////////////////////
    Type getType() override;


    //////////////////////////////////////////////////////
    ///@brief Print already calculated value of the formulaCell.
    ///
    //////////////////////////////////////////////////////
    void print() const override;


    //////////////////////////////////////////////////////
    ///@brief Get the spacing required for printing the calculated value of the formulaCell object.
    ///
    ///@return The spacing required for printing the calculated value of the formulaCell object.
    //////////////////////////////////////////////////////
    size_t getSpacing() override;


    //////////////////////////////////////////////////////
    ///@brief Get the result of the expression calculation of formulaCell's value 
    ///
    ///@return The result of the expression calculation of formulaCell's value
    //////////////////////////////////////////////////////
    double getNum_Value() override;


    //////////////////////////////////////////////////////
    ///@brief Get the value of the formulaCell in form of a string expression.
    ///
    ///@return The value of the formulaCell in form of a string expression.
    //////////////////////////////////////////////////////
    std::string getS_Value() const override; 

    
    //////////////////////////////////////////////////////
    ///@brief Re-fills the vector containing pointers to the cells which the current cell depends on. 
    ///
    //////////////////////////////////////////////////////
    void fill_dependingOn();


    //////////////////////////////////////////////////////
    ///@brief Get the vector containing pointers to the cells which the current cell depends on.
    ///
    ///@return Const reference to the vector containing pointers to the cells which the current cell depends on.
    //////////////////////////////////////////////////////
    const std::vector<Cell**>& getDependingOn();


    //////////////////////////////////////////////////////
    ///@brief In some cases there may occur infinite recursion (infinite cell referencing)
    ///       which leads to crash. Check if the current formulaCell is such case.
    ///       If yes, throw an exception... This method is recursive itself, checking
    ///       the cell-depending graph.
    ///
    ///@param cell Pointer to the cell we check in the moment.
    ///@param forbiddenCells If the dependingOn vector of the current cell has any common members with this vector, infinite cell referencing is found.
    //////////////////////////////////////////////////////
    void checkForRecursion(formulaCell* cell, std::vector<Cell*>& forbiddenCells); 
    
    
    //////////////////////////////////////////////////////
    ///@brief Calculate the formulaCell expression value.
    ///
    ///@return The result of the formulaCell expression value in type double. 
    //////////////////////////////////////////////////////
    double calculate();


    //////////////////////////////////////////////////////
    ///@brief The original expression value may contain some references to other cells... 
    ///       Check that and replace these cell references with the respective double type values.
    ///
    ///@return An only number-and-operator expression which can be calculated by shunting yard algorithm. 
    //////////////////////////////////////////////////////
    std::string getCalculationExpression();


    //////////////////////////////////////////////////////
    ///@brief Check if a char is one of + - / * ^
    ///
    ///@param c Random variable of type char
    ///@return True if c is + - / * or ^ 
    //////////////////////////////////////////////////////
    static bool isOperator (char c);


    //////////////////////////////////////////////////////
    ///@brief Check if a char is digit.
    ///
    ///@param c Random variale of type char.
    ///@return True if c is a digit. 
    //////////////////////////////////////////////////////
    static bool isDigit (char c);


    //////////////////////////////////////////////////////
    ///@brief There is an operator hierarchy in the shunting yard algorithm.
    ///       Level 1: '^' ; Level 2: '/' and '*' ; Level 3: '+' and '-'. 
    ///
    ///@param c Random variable of type char.
    ///@param top Random variable of type char.
    ///@return True if c is higher than top in the operator hierarchy.
    //////////////////////////////////////////////////////
    static bool isBiggerOperator(char c, char top);


    //////////////////////////////////////////////////////
    ///@brief Shunting Yard algorithm. Get the value a valid string formed expression leads to.
    ///
    ///@param exp Valid only number-and-operator expression.
    ///@return The result of the calculation of the expression. 
    //////////////////////////////////////////////////////
    static double shuntingYard(const std::string& exp);
    
};