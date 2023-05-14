#include "../headers/commands.h"
#include <iostream>

Commands::Commands()
{
    table = nullptr;
    dataSaved = true;
}


Commands::~Commands()
{
    delete table;
}



const std::string& Commands::getPath() const
{
    return path;
}



bool Commands::isThereTable() const
{
    return (table != nullptr);
}


void Commands::NEW()
{
    CLOSE();
    table = new Table();
    dataSaved = true;
    path.push_back('\0'); //means we have a doc but it does not have a path yet
    std::cout << "New document created successfully!\n";
}



void Commands::OPEN(const std::string& path)
{
    CLOSE();
    std::ifstream file(path);
    if (!file.is_open()){
        throw std::runtime_error("Error loading file! You have probably chosen an unexisting file!");
    }

    try {
        table = new Table(file);
        file.close();
        this->path = path;
        dataSaved = true;
        std::cout << "Successfully opened " << path << std::endl;
    } catch (const std::invalid_argument& e){
        std::cerr << e.what() << std::endl;
        CLOSE();
    }
    
}



void Commands::SAVE()
{
    if (!table){
        throw std::invalid_argument("Error: no document is currently opened\nHint: open an existing file, or create a new document first.");
    }

    if (path.front() == '\0'){
        throw std::logic_error("ERROR: No file chosen. \nHint: Use \"SaveAs <file_name>\" to save the Document");
    }

    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()){
        throw std::runtime_error("Error opening file!");
    }
    table->saveInFile(file);
   
    dataSaved = true;
    file.close();
    std::cout << "Successfully saved to " << path << std::endl;
}



void Commands::SAVEAS(const std::string& path)
{
    if (!table){
        throw std::invalid_argument("Error: no document is currently opened\nHint: open an existing file, or create a new document first.");
    }

    //check if there is already a file with that path 
    std::ifstream check(path);
    if (check.is_open()){
        std::cout << "You have chosen an existing file. Do you want to overwrite it? (Y/N)\n";
        char choice;
        do {
            std::cin >> choice;
            std::cin.get(); //to get the '\n' symbol
        } while (choice!='Y' && choice!='y' && choice!='N' && choice!='n');

        if (choice == 'N' || choice == 'n'){
            std::cout << "Operation canceled!" << std::endl;
            return;
        }
        check.close();
    }

    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()){
        throw std::runtime_error("Error opening file!");
    }
    table->saveInFile(file);

    this->path = path;
    dataSaved = true;
    file.close();
    std::cout << "Successfully saved to " << path << std::endl;
}



void Commands::CLOSE()
{
    if (!table){ 
        return;
    }

    if (!dataSaved){
        std::cout << "File not saved! Do you want to save it before closing? (Y/N)\n";
        char choice;
        do {
            std::cin >> choice;
            std::cin.get(); //to get the '\n' symbol
        } while (choice!='Y' && choice!='y' && choice!='N' && choice!='n');

        if (choice == 'Y' || choice == 'y'){
            SAVE();
        }
    }

    delete table;
    table = nullptr;
    path.clear();

}



void Commands::GET(const std::string& cellAddress)
{
    if (!table){
        throw std::invalid_argument("Error: no document is currently opened\nHint: open an existing file, or create a new document first.");
    }

    char col;
    size_t row;

    readCellAddress(cellAddress, col, row); //throws if address is not valid

    Cell** found = table->getCell(col, row);
    if (!found){
        std::cout << col << row << " has a value of 0\n";
        return;
    }

    std::string value_of_cell = (*found)->getS_Value();
    if (value_of_cell.size() == 0){
        std::cout << col << row << " is an empty cell" << std::endl; 
    }
    else {
        std::cout << col << row << " has a value of " << (*table->getCell(col, row))->getS_Value() << std::endl;
    }
}



void Commands::EDIT(const std::string& cellAddress, std::string& newValue)
{
    if (!table){
        throw std::invalid_argument("Error: no document is currently opened\nHint: open an existing file, or create a new document first.");
    }
    
    char col;
    size_t row;
    readCellAddress(cellAddress, col, row); //throws if address is not valid

    table->setValue(col, row, newValue);
    dataSaved = false;
    std::cout << col << row << " successfully set to " << newValue << std::endl;
}



void Commands::PRINT()
{
    if (!table){
        throw std::invalid_argument("Error: no document is currently opened\nHint: open an existing file, or create a new document first.");
    }
    table->print();
}



void Commands::EXIT()
{
    CLOSE();
    std::cout << "Exiting the program...\n";
}



void Commands::readCellAddress(const std::string& cellAddress, char& col, size_t& row)
{
    if (cellAddress.size() < 2 || (cellAddress[1] < '0' || cellAddress[1] > '9') ){
        throw std::invalid_argument("Invalid cell!");
    }

    col = cellAddress[0];
    if (col >= 'a' && col <= 'z'){
        col -= 'a' - 'A';
    }

    if (col < 'A' || col > 'Z'){
        throw std::invalid_argument("Invalid cell!");
    }

    std::string stringed_row(cellAddress);
    stringed_row.erase(0,1);
    
    try {
        row = std::stoi(stringed_row);
    } catch (const std::exception& e){
        throw std::invalid_argument("Invalid cell!");
    }

    if (row == 0){
        throw std::invalid_argument("Invalid cell!");
    }
}