#include "../headers/table.h"
#include <iostream>

Table::Table()
{
    longestRow = 0;
}


Table::Table(std::ifstream& file)
{
    longestRow = 0;
    readFromFile(file);
}


Table::~Table()
{
    for (size_t i=0; i<cells.size(); ++i){
        for (size_t j=0; j<cells[i].size(); ++j){
            delete cells[i][j];
        }
        cells[i].clear();
    }
    cells.clear();
}



void Table::align()
{
    spacing.clear();

    for (size_t i=0; i<cells.size(); ++i){
        while (cells[i].size() < longestRow){
            Cell* fill = new emptyCell;
            cells[i].push_back(fill);
        }
    }

    //going through columns
    for (size_t j=0; j<longestRow; ++j){
        
        unsigned columnLongest = 0;
        for (size_t i=0; i<cells.size(); ++i){
            if (cells[i][j]->getSpacing() > columnLongest){
                columnLongest = cells[i][j]->getSpacing();
            }
        }
        spacing.push_back(columnLongest);

    }
    
}



void Table::print()
{
    if (cells.size() == 0){
        throw std::logic_error("The document is empty");
    }

    align();

    //gets the spacing required for the longest (biggest) row number 
    size_t rowNumsSpacing = std::to_string(cells.size()).size();
    
    std::cout << " ";
    for (size_t i=0; i<rowNumsSpacing; ++i){
        std::cout << " ";
    }
    std::cout << " | ";

    for (size_t i=0; i<longestRow; ++i){
        std::cout << char ('A' + i);

        size_t emptySpacing = spacing[i] ? spacing[i] - 1 : 0;

        for (size_t i=0; i<emptySpacing; ++i)
                std::cout << " "; 
        std::cout << " | ";
    }
    std::cout << "\n";
    
    for (size_t i=0; i<cells.size(); ++i){
        
        std::cout << " ";

        for (size_t k=0; k<rowNumsSpacing - std::to_string(i+1).size(); ++k){
            std::cout << " ";
        }

        std::cout << i + 1 << " | ";
    
        for (size_t j=0; j<cells[i].size(); ++j){

            size_t emptySpacing = spacing[j] ? spacing[j] - cells[i][j]->getSpacing() : 1;
        
            for (size_t i=0; i<emptySpacing; ++i)
                std::cout << " ";
            
            cells[i][j]->print();
            std::cout << " | ";
        }

        std::cout << std::endl;
    }
}



void Table::addRow (const std::string& row)
{
    std::vector <Cell*> newRow;
    size_t read = 0;
    
    for (read; read<row.size(); ++read){
        
        std::string value;
        while (row[read] == ' ' && read<row.size()) ++read;

        while (row[read] != ',' && read<row.size()) {
            value.push_back(row[read]);
            ++read;
        }

        while (value.size() > 0 && value.back() == ' '){
            value.pop_back();
        }

        int type = whatIsThis(value);

        //Error cell
        if (type == 0){
            size_t ErrorRow = cells.size() + 1;
            size_t ErrorCol = newRow.size() + 1;
            std::string errMes("Error: row "); //Error Message
            errMes += std::to_string(ErrorRow) + ", col " + std::to_string(ErrorCol) + ", " + value + " is unknown data type"; 
            throw std::invalid_argument(errMes);
        }

        
        //string
        else if (type == 1){
            Cell* newPtr = new stringCell(value);
            newRow.push_back(newPtr);
        }

        //integer
        else if (type == 2){
            if (value[0] == '+'){
                value.erase(value.begin()); //delete the + symbol
            }

            try {
                Cell* newPtr = new intCell(stoi(value));
                newRow.push_back(newPtr);
            } catch(const std::exception& e){
                throw std::invalid_argument("Number too Big!");
            }
            
        }

        //double
        else if (type == 3){ 
            if (value[0] == '+'){
                value.erase(value.begin()); //delete the + symbol
            }

            try {
                Cell* newPtr = new doubleCell(stod(value));
                newRow.push_back(newPtr);
            } catch (const std::exception& e){
                throw std::invalid_argument("Number too Big!");
            }
            
        }

        //empty
        else if (type == 4){ 
            Cell* newPtr = new emptyCell;
            newRow.push_back(newPtr);
        }

        //formula
        else { //type == 5
            Cell* newPtr = new formulaCell(value, this);
            newRow.push_back(newPtr);
        }

    }

    if (newRow.size() > longestRow){
        longestRow = newRow.size();
    }

    cells.push_back(newRow);
}



void Table::setValue(char col, size_t row, std::string& newValue)
{
    size_t column = size_t (col - 'A');

    int newType = whatIsThis(newValue);
    if (newType == 0){
        throw std::invalid_argument("Error: incorrect value " + newValue);
    }
        
    //adding empty rows if necessary
    while (row > cells.size()){
        addRow("");
    }

    //adding empty columns if necessary
    while (column >= cells[row-1].size()){
        Cell* empty_Cell = new emptyCell;
        cells[row-1].push_back(empty_Cell);
    }

    if (longestRow <= column){
        longestRow = column+1;
    }

    Cell* newCell;
    switch (newType){
        case 1: newCell = new stringCell(newValue); break;

        case 2: newCell = new intCell(std::stoi(newValue)); break;
        
        case 3: newCell = new doubleCell(std::stod(newValue)); break;

        case 5: newCell = new formulaCell(newValue, this); break;
        
        default: throw std::runtime_error("Unexpected error occured!");
    }

    delete cells[row-1][column];
    cells[row-1][column] = newCell;
}



Cell** Table::getCell (char col, size_t row)
{
    size_t column = size_t (col - 'A');

    if (row > cells.size() || column >= cells[row-1].size()){
        return nullptr;
    }
    
    return &cells[row-1][column];
}



void Table::readFromFile (std::ifstream& file)
{
    while (!file.eof()){
        std::string row;
        getline (file, row);
        addRow(row);
    }
}



void Table::saveInFile (std::ofstream& file)
{
    align();
    for (size_t i=0; i<cells.size(); ++i){
        std::string rowValue;
        for (size_t j=0; j<cells[i].size(); ++j){
            rowValue += cells[i][j]->getS_Value();
            if (j+1 < cells[i].size()){
                rowValue.push_back(',');
            }
        }
        file << rowValue;
        if (i+1<cells.size()){
            file << "\n";
        }
    }
    
}



int Table::whatIsThis (std::string& str)
{
    if (str.size() == 0){
        return 4;
    }

    if (str[0] == '='){
        return isFormula(str) ? 5 : 0;
    }



    size_t read = 0;

    //string check
    if (str[read] == '"'){

        if (str.back() != '"' || str.size() < 2)
            return 0;

        for (++read; read<str.size()-1; ++read){

            if (str[read] == '"'){
                return 0;
            }

            if (str[read] == '\\'){
                ++read;
                if (read >= str.size()-1 || (str[read] != '\\' && str[read] != '"')){
                    return 0;
                }
            }

        }
    
        return 1;
    }


    bool hadPoint = false;

    if (str[0] == '.' || ((str[0] == '+' || str[0] == '-') && (str.size() == 1 || str[1] == '.'))){
        return 0;
    }

    //last symbol must be a digit
    if (str.back() < '0' || str.back() > '9'){
        return 0;
    }

    for (++read; read<str.size()-1; ++read){
        if ((str[read] < '0' || str[read] > '9') && str[read] != '.')
            return 0;            
        
        else if (str[read] == '.' && !hadPoint)
            hadPoint = true;

        else if (str[read] == '.' && hadPoint)
            return 0; 
    }

    return hadPoint ? 3 : 2;
}



bool Table::isFormula(std::string& str)
{
        //deleting empty spaces and to upper case
        for (size_t i=0; i<str.size(); ++i){
            if (str[i] == ' '){
                str.erase(i,1);
                --i;
            }

            if (str[i] >= 'a' && str[i] <= 'z'){
                str[i] -= 'a' - 'A';
            }
        }

        if (str.size() < 2){
            return false;
        }

        bool lastSign = false; //true if the last read char is an operator
        for (size_t i=1; i<str.size(); ++i){

            bool hasPoint = false; //true if the current read number has point 
            if (str[i] >= 'A' && str[i] <= 'Z'){
                ++i;
                bool invalid = true;
                while (i<str.size() && !formulaCell::isOperator(str[i])){
                    if (formulaCell::isDigit(str[i]) && str[i] != '0'){
                        invalid = false;
                    }
                    ++i;
                }
                --i;
                if (invalid){
                    return false;
                }
                lastSign = false;
            }

            else if (formulaCell::isOperator(str[i])){
    
                //3 operators cannot be one after another
                //2 operators could be one after another only if the second one is + or -
                if (lastSign && ( formulaCell::isOperator(str[i-2]) || str[i] == '*' || str[i] == '/' || str[i] == '^')){
                    return false; 
                }
                
                lastSign = true;
            }


            else if (formulaCell::isDigit(str[i])){
                lastSign = false;
                hasPoint = false;
                continue;
            }


            else if (str[i] == '.'){
                if (hasPoint || lastSign || str.size() == i+1 || i == 1){
                    return false;
                }
                hasPoint = true;
            }


            else { //str[i] is invalid symbol
                return false;
            }
        }

        return !lastSign;
}
