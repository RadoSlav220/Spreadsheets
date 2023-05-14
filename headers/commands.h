#pragma once
#include "table.h"


//////////////////////////////////////////////////////
///@brief Here are being executed all commands.
///
//////////////////////////////////////////////////////
class Commands {

private:

    //////////////////////////////////////////////////////
    ///@brief Pointer to a table to work with. Nullptr if we don't work with any document.
    ///
    //////////////////////////////////////////////////////
    Table* table;

    //////////////////////////////////////////////////////
    ///@brief Path leading to the file we are working with.
    ///       Empty string if we don't work with any document. 
    ///       Has value of '\0' if we work with a new document which has no path.
    ///
    //////////////////////////////////////////////////////
    std::string path;

    //////////////////////////////////////////////////////
    ///@brief False if changes are made and not saved yet.
    ///
    //////////////////////////////////////////////////////
    bool dataSaved;

public:

    //////////////////////////////////////////////////////
    ///@brief Construct a new Commands object. No created document yet.      
    ///
    //////////////////////////////////////////////////////
    Commands();

    Commands (const Commands&) = delete;

    Commands& operator= (const Commands&) = delete;

    //////////////////////////////////////////////////////
    ///@brief Destroy the Commands object
    ///
    //////////////////////////////////////////////////////
    ~Commands();


    //////////////////////////////////////////////////////
    ///@brief Get the path leading to the document we work with. 
    ///
    ///@return The path leading to the document we work with. 
    //////////////////////////////////////////////////////
    const std::string& getPath() const;


    //////////////////////////////////////////////////////
    ///@brief Check if there is working document.
    ///
    ///@return True if there is document we are working with. 
    //////////////////////////////////////////////////////
    bool isThereTable() const;


    //////////////////////////////////////////////////////
    ///@brief Close the old document if there is such and create a new one.
    ///
    //////////////////////////////////////////////////////
    void NEW();


    //////////////////////////////////////////////////////
    ///@brief Close the old document if there is such and open a document. If there is problem with the data types, show what is wrong.
    ///
    ///@param path Path to a document.
    //////////////////////////////////////////////////////
    void OPEN(const std::string& path);


    //////////////////////////////////////////////////////
    ///@brief Close the current document. If data is not saved, ask whether to save it or not.
    ///
    //////////////////////////////////////////////////////
    void CLOSE();


    //////////////////////////////////////////////////////
    ///@brief Save data to the document path leads to. If path does not lead anywhere, throw an exception.
    ///       If there is no current document, throw an exception.
    ///
    //////////////////////////////////////////////////////
    void SAVE();


    //////////////////////////////////////////////////////
    ///@brief Save the document to a new path. If the chosen path leads to already existing file, ask whether to overwrite it or not.
    ///       If there is no current document, throw an exception.
    ///
    ///@param path Path to file. May be unexisting one.
    //////////////////////////////////////////////////////
    void SAVEAS(const std::string& path);


    //////////////////////////////////////////////////////
    ///@brief Close the current document and exit.
    ///
    //////////////////////////////////////////////////////
    void EXIT();



    //////////////////////////////////////////////////////
    ///@brief Get the value of a cell. If there is no current document, throw an exception.
    ///
    ///@param cellAddress Reference to a cell in the table.
    //////////////////////////////////////////////////////
    void GET(const std::string& cellAddress);


    //////////////////////////////////////////////////////
    ///@brief Edit the value in a cell. If there is no current document, throw an exception.
    ///
    ///@param cellAddress Reference to a cell in the table. May be out of the current table limits.
    ///@param newValue The new value. If it is not correct, Table::setValue() throws an exception.
    //////////////////////////////////////////////////////
    void EDIT(const std::string& cellAddress, std::string& newValue);
    
    
    //////////////////////////////////////////////////////
    ///@brief Print the table. If there is no current document, throw an exception.
    ///
    //////////////////////////////////////////////////////
    void PRINT();

private:

    //////////////////////////////////////////////////////
    ///@brief Read cell Address. If it is invalid, throw an exception.
    ///
    ///@param cellAddress The cell address. May be out of the current table limits. 
    ///@param col Uninitialized char. The method assigns the column letter to it. 
    ///@param row Uninitialized size_t. The method assigns the row number to it.
    //////////////////////////////////////////////////////
    void readCellAddress(const std::string& cellAddress, char& col, size_t& row);

};