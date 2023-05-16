#include "../headers/program.h"
#include <iostream>

Program::Program ()
{
    wantToExit = false;
}

void Program::Go()
{
    while (!wantToExit){
        std::string command;
        std::cout << ">";
        getline(std::cin, command);
        try {
            executeCommand(command);
        } catch (const std::exception& e){
            std::cerr << e.what() << std::endl;
        }
        std::cout << std::endl; 
    }
}



std::string Program::getPartOfCmd(const std::string& command, size_t& read)
{
    if (read >= command.size()){
        throw std::out_of_range("End of command reached");
    }

    std::string part;
    while (read < command.size() && command[read] == ' '){
        ++read;
    }

    if (read < command.size() && (command[read] == '=' || command[read] == '"')){
        part.assign(command.begin() + read, command.end());
        read = command.size();
        return part;
    }

    while (read < command.size() && command[read] != ' '){
        part.push_back(command[read]);
        ++read;
    } 

    return part;
}



bool Program::isThereExcess(const std::string& command, size_t& read)
{
    while (read < command.size()){
        if (command[read] != ' ') 
            return true; 
        ++read;
    }
    return false;
}



void Program::toLowerCase(std::string& str)
{
    for (size_t i=0; i<str.size(); ++i){
        if (str[i] >= 'A' && str[i] <= 'Z'){
            str[i] += 'a' - 'A';
        }
    }
}


void Program::executeCommand(const std::string& command)
{
    size_t read = 0;
    std::string cmdName, firstArg, secondArg;
    
    //First we have to separate the command into pieces:
    try {
        cmdName = getPartOfCmd(command, read);
        firstArg = getPartOfCmd(command, read);
        secondArg = getPartOfCmd(command, read);
    } catch (const std::out_of_range& e){}

    //if there is command name and more than 2 arguments, the command is invalid
    if (isThereExcess(command, read)){
        throw std::invalid_argument("Invalid command!");
    }

    toLowerCase(cmdName); //so save, Save, SaVe, SAVE are the same thing
    

    //Searching for matching command Name
    if (cmdName == "new"){
        if (firstArg.size() != 0){
            throw std::invalid_argument("Invalid command!");
        }
        commands.NEW();
    }



    else if (cmdName == "open"){

        if (firstArg.size() == 0 || secondArg.size() != 0){
            throw std::invalid_argument("Invalid command!");
        }

        //to get rid of quotation marks
        if (firstArg.front() == '"' && firstArg.back() == '"'){
            firstArg.erase(0,1);
            firstArg.pop_back();
        }

        commands.OPEN(firstArg);
    }


    else if (cmdName == "close"){
        if (firstArg.size() != 0){
            throw std::invalid_argument("Invalid command!");
        }

        if (!commands.isThereTable()){
            throw std::logic_error("ERROR: Cannot close unexisting document");
        }

        std::string path_to_close = commands.getPath();
        commands.CLOSE();

        std::cout << "Successfully closed ";
        if (path_to_close.size() == 1 && path_to_close[0] == '\0'){
            std::cout << "New Document";
        }
        else {
            std::cout << path_to_close;
        }
        std::cout << std::endl;
    }



    else if (cmdName == "save"){

        if (firstArg.size() != 0){
            throw std::invalid_argument("Invalid command!");
        }
        
        commands.SAVE();
    }



    else if (cmdName == "saveas"){

        if (firstArg.size() == 0 || secondArg.size() != 0){
            throw std::invalid_argument("Invalid command!");
        }
        
        //to get rid of quotation marks
        if (firstArg.front() == '"' && firstArg.back() == '"'){
            firstArg.erase(0,1);
            firstArg.pop_back();
        }

        commands.SAVEAS(firstArg);
    }



    else if (cmdName == "get"){

        if (firstArg.size() == 0 || secondArg.size() != 0){
            throw std::invalid_argument("Invalid command!");
        }

        commands.GET(firstArg);
    }



    else if (cmdName == "edit"){
        
        if (firstArg.size() == 0 || secondArg.size() == 0){
            throw std::invalid_argument("Invalid command!");
        }

        if (firstArg[0] >= 'a' && firstArg[0] <= 'z'){
            firstArg[0] -= 'a' - 'A';
        }

        commands.EDIT(firstArg, secondArg);
    }



    else if (cmdName == "print"){
        
        if (firstArg.size() != 0){
            throw std::invalid_argument("Invalid command!");
        }
        commands.PRINT();
    }



    else if (cmdName == "help"){
        
        if (firstArg.size() != 0){
            throw std::invalid_argument("Invalid command!");
        }
        commands.HELP();
    }



    else if (cmdName == "exit"){
        
        if (firstArg.size() != 0){
            throw std::invalid_argument("Invalid command!");
        }
        commands.EXIT();
        wantToExit = true;
    }
    


    else {
        throw std::invalid_argument("Invalid command!");
    }
}