#pragma once
#include "cell.h"
#include "formulaCell.h"
#include <vector>
#include <fstream>


//////////////////////////////////////////////////////
///@brief Stores and works with all cells. The main class of the project.
///
//////////////////////////////////////////////////////
class Table {

private:

    //////////////////////////////////////////////////////
    ///@brief Stores pointers to all cells in the table.
    ///
    //////////////////////////////////////////////////////
    std::vector < std::vector <Cell*> > cells;

    //////////////////////////////////////////////////////
    ///@brief Stores width of the columns so the table can be printed aligned.
    ///
    //////////////////////////////////////////////////////
    std::vector <unsigned> spacing;

    //////////////////////////////////////////////////////
    ///@brief Stores the size of the longest row of the table.
    ///
    //////////////////////////////////////////////////////
    size_t longestRow;

public:

    //////////////////////////////////////////////////////
    ///@brief Construct a new empty Table object
    ///
    //////////////////////////////////////////////////////
    Table();


    //////////////////////////////////////////////////////
    ///@brief Construct a new Table object with already added data.
    ///
    ///@param file File to read from.
    //////////////////////////////////////////////////////
    Table(std::ifstream& file);


    //////////////////////////////////////////////////////
    ///@brief Destroy the Table object
    ///
    //////////////////////////////////////////////////////
    ~Table();


    //////////////////////////////////////////////////////
    ///@brief Print the aligned table.
    ///
    //////////////////////////////////////////////////////
    void print();


    //////////////////////////////////////////////////////
    ///@brief Align the table.
    ///
    //////////////////////////////////////////////////////
    void align();


    //////////////////////////////////////////////////////
    ///@brief Add a new row to the table. If the row contains a cell
    ///       with unknown data type, throw an exception with a message what is wrong.
    ///
    ///@param row The new row.
    //////////////////////////////////////////////////////
    void addRow (const std::string& row);


    //////////////////////////////////////////////////////
    ///@brief Change the value of a cell. If the new value is incorrect, throw an exception.
    ///
    ///@param col The column of the cell, we want to edit. Must be upper case letter! May be out of the current table limits.
    ///@param row The row of the cell we want to edit. Must be positive. May be out of the current table limits.
    ///@param newValue The new value. 
    //////////////////////////////////////////////////////
    void setValue(char col, size_t row, std::string& newValue);


    //////////////////////////////////////////////////////
    ///@brief Get double pointer to a cell.
    ///
    ///@param col The column of the cell we want to get access to. Must be upper case letter! May be out of the current table limits.
    ///@param row The row of the cell we want to get access to. Must be positive! May be out of the current table limits.
    ///@return Pointer to poiner to a cell in the table or nullptr if the cell is out of the current table limits.
    //////////////////////////////////////////////////////
    Cell** getCell (char col, size_t row);


    //////////////////////////////////////////////////////
    ///@brief Read data from file.
    ///
    ///@param file File to read data from.
    //////////////////////////////////////////////////////
    void readFromFile (std::ifstream& file);


    //////////////////////////////////////////////////////
    ///@brief Save data in file.
    ///
    ///@param file File to save data in.
    //////////////////////////////////////////////////////
    void saveInFile (std::ofstream& file);



    //////////////////////////////////////////////////////
    ///@brief Determine the type of the argument.
    ///
    ///@param str Random string.
    ///@return 0 for unknown type ;   
    ///        1 for string ;
    ///        2 for int ;
    ///        3 for double ; 
    ///        4 for empty type ; 
    ///        5 for formula.
    //////////////////////////////////////////////////////
    int whatIsThis(std::string& str);

private:

    //////////////////////////////////////////////////////
    ///@brief Check if a string is formula or not.
    ///
    ///@param str Random string.
    ///@return True if ther string is formula type. 
    //////////////////////////////////////////////////////
    bool isFormula(std::string& str);

};