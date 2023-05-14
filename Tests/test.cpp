#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "../headers/cell.h"
#include "../headers/formulaCell.h"
#include "../headers/table.h"
#include "../headers/commands.h"
#include "../headers/program.h"
#include <iostream>



TEST_CASE ("Testing intCell")
{
    intCell cell(123);
    REQUIRE (cell.getType() == Type::INT);
    REQUIRE (cell.getValue() == 123);
    REQUIRE (cell.getS_Value() == "123");
    REQUIRE (cell.getNum_Value() - 123 < 0.0001);
    REQUIRE (cell.getSpacing() == 3);
}



TEST_CASE ("Testing doubleCell")
{
    doubleCell cell(12.123);
    REQUIRE (cell.getType() == Type::DOUBLE);
    REQUIRE (cell.getValue() == 12.123);
    REQUIRE (cell.getS_Value() == "12.123");
    REQUIRE (cell.getNum_Value() - 12.123 < 0.000001);
    REQUIRE (cell.getSpacing() == 6);
}



TEST_CASE ("Testing stringCell")
{
    // Hello "World"\There                   -> the printing form of the string
    // "Hello \"World\"\\There"              -> the string in c/c++ form
    stringCell cell("\"Hello \\\"World\\\"\\\\There\"");
    REQUIRE (cell.getType() == Type::STRING);
    REQUIRE (cell.getValue() == "Hello \"World\"\\There");
    REQUIRE (cell.getS_Value() == "\"Hello \\\"World\\\"\\\\There\"");
    REQUIRE (cell.getNum_Value() - 0 < 0.00001);
    REQUIRE (cell.getSpacing() == 19);

    stringCell cell2("\"1234\"");
    REQUIRE (cell2.getNum_Value() - 1234 < 0.00001);
    REQUIRE (cell2.getSpacing() == 4);

    stringCell cell3("\"0.9\"");
    REQUIRE (cell3.getNum_Value() - 0.9 < 0.00001);
    REQUIRE (cell3.getSpacing() == 3);

}



TEST_CASE ("Testing emptyCell")
{
    emptyCell cell;
    REQUIRE (cell.getType() == Type::EMPTY);
    REQUIRE (cell.getS_Value() == "");
    REQUIRE (cell.getNum_Value() - 0 < 0.000001);
    REQUIRE (cell.getSpacing() == 0);
}



TEST_CASE ("Testing formulaCell")
{

    SECTION ("Testing the small static methods")
    {
        REQUIRE (formulaCell::isDigit('0') == true);
        REQUIRE (formulaCell::isDigit('5') == true);
        REQUIRE (formulaCell::isDigit('a') == false);
        REQUIRE (formulaCell::isOperator('+') == true);
        REQUIRE (formulaCell::isOperator('^') == true);
        REQUIRE (formulaCell::isOperator('p') == false);
        REQUIRE (formulaCell::isOperator('5') == false);
        REQUIRE (formulaCell::isBiggerOperator('*', '+') == true);
        REQUIRE (formulaCell::isBiggerOperator('/', '-') == true);
        REQUIRE (formulaCell::isBiggerOperator('^', '+') == true);
        REQUIRE (formulaCell::isBiggerOperator('^', '/') == true);
        REQUIRE (formulaCell::isBiggerOperator('-', '+') == false);
        REQUIRE (formulaCell::isBiggerOperator('*', '*') == false);
        REQUIRE (formulaCell::isBiggerOperator('+', '^') == false);
    }


    SECTION ("Basic tests")
    {
        //first we have to create table
        std::string row1("12, 0.8, 123");
        std::string row2(",220,\"100\"");
        // 12 | 0.8 | 123
        //    | 220 | 100
        Table t;
        t.addRow(row1);
        t.addRow(row2);

        formulaCell cell("=a1+B1*100", &t);
        REQUIRE (cell.getType() == Type::FORMULA);
        REQUIRE (cell.getDependingOn().size() == 0); //in the beginning dependingOn vector should be empty
        REQUIRE (cell.getS_Value() == "=a1+B1*100");
    }


    SECTION ("Testing Shunting Yard algorithm")
    {
        #define calc formulaCell::shuntingYard 
        REQUIRE (calc("5+5") == 10.0);
        REQUIRE (calc("1/2") == 0.5);
        REQUIRE (calc("10+81/3^2") == 19.0);
        REQUIRE (calc("13.5+1/2") == 14.0);
        REQUIRE (calc("3+4*2/-4^2") == 3.5);
        REQUIRE_THROWS (calc("10.5/0"));
        #undef calc
    }


    SECTION ("Doing undepending calculations")
    {
        Table t;
        formulaCell cell("=45+5", &t);
        REQUIRE (cell.calculate() == 50);

        formulaCell cell2("=12.5+1/2-14", &t);
        REQUIRE (cell2.calculate() == -1);
    }


    SECTION ("Testing simple depending calculations")
    {
        //first we have to create table
        std::string row1("12, 0.8, 123");
        std::string row2(",220,\"100\"");
        // 12 | 0.8 | 123
        //    | 220 | 100
        Table t;
        t.addRow(row1);
        t.addRow(row2);

        formulaCell cell("=2^A1",&t);
        REQUIRE (cell.getCalculationExpression() == "2^12.000000");
        REQUIRE (cell.calculate() == 4096);
        REQUIRE (cell.getNum_Value() == 4096);

        formulaCell cell2("=C2*B1+A1",&t);
        REQUIRE (cell2.getCalculationExpression() == "100.000000*0.800000+12.000000");
        REQUIRE (cell2.calculate() == 92);

        formulaCell cell3("=A1/A2", &t);
        REQUIRE (cell3.getCalculationExpression() == "12.000000/0.000000");
        REQUIRE_THROWS (cell3.getNum_Value());

        formulaCell cell4("=C2*B1+A1/A220", &t);
        REQUIRE_THROWS (cell4.getNum_Value());
    }
    

    SECTION ("Testing not that simple depending calculations")
    {
        //first we have to create table
        std::string row1("=B1*C2, 0.8, 123");
        std::string row2(",220,\"100\"");
        // 80 | 0.8 | 123
        //    | 220 | 100
        Table t;
        t.addRow(row1);
        t.addRow(row2);

        formulaCell cell("=A1+B2",&t);
        cell.fill_dependingOn();
        REQUIRE (cell.getDependingOn().size() == 2);
        REQUIRE (cell.getNum_Value() == 300);

        formulaCell cell2("=A1*C2+B1", &t);
        REQUIRE (cell2.getNum_Value() == 8000.8);
    }


    SECTION ("Testing infinite calculations")
    {
        //first we have to create table
        std::string row1("=B1*C2, =A1+C2, 123");
        std::string row2(",220,\"100\"");
        // #ERROR | #ERROR | 123
        //        |  220   | 100
        Table t;
        t.addRow(row1);
        t.addRow(row2);

        formulaCell cell("=A1*2", &t);
        REQUIRE_THROWS (cell.getNum_Value());
    }
}



TEST_CASE ("Testing table")
{
    SECTION ("Recognizing different type of values")
    {
        Table t;
        std::string test("\"Hello\"");
        REQUIRE (t.whatIsThis(test) == 1);
        
        test = "123";
        REQUIRE (t.whatIsThis(test) == 2);

        test = "123.123";
        REQUIRE (t.whatIsThis(test) == 3);

        test = "123.123.123";
        REQUIRE (t.whatIsThis(test) == 0);

        test = "=A1+B2*5";
        REQUIRE (t.whatIsThis(test) == 5);

        test = "=    g12  *2  -0.45";
        REQUIRE (t.whatIsThis(test) == 5);

        test.clear();
        REQUIRE (t.whatIsThis(test) == 4);
    }

    SECTION ("Adding new rows")
    {
        Table t;
        std::string row1("\"Hello world\", 123, 0.77");
        REQUIRE_NOTHROW (t.addRow(row1));

        std::string row2(", invalid, 0.77");
        REQUIRE_THROWS (t.addRow(row2));

        std::string row3(", =14*14+A0, 0.77"); //A0 does not name a correct cell
        REQUIRE_THROWS (t.addRow(row3));
    }


    SECTION ("Accessing cells")
    {
        Table t;
        std::string row1("=B1*C2, 0.8, \"Hey\"");
        std::string row2(",220,\"100\"");
        // 80 | 0.8 | Hey
        //    | 220 | 100
        t.addRow(row1);
        t.addRow(row2);

        Cell** cellptr = t.getCell('B', 2);
        REQUIRE ( (*cellptr)->getType() == Type::INT );
        REQUIRE ( (*cellptr)->getNum_Value() == 220);
        REQUIRE ( (*cellptr)->getS_Value() == "220");
        REQUIRE ( (*cellptr)->getSpacing() == 3);

        cellptr = t.getCell('A', 2);
        REQUIRE ( (*cellptr)->getType() == Type::EMPTY );
        REQUIRE ( (*cellptr)->getNum_Value() == 0);
        REQUIRE ( (*cellptr)->getS_Value() == "");
        REQUIRE ( (*cellptr)->getSpacing() == 0);

        cellptr = t.getCell('A', 1);
        REQUIRE ( (*cellptr)->getType() == Type::FORMULA );
        REQUIRE ( (*cellptr)->getNum_Value() == 80);
        REQUIRE ( (*cellptr)->getS_Value() == "=B1*C2");
        REQUIRE ( (*cellptr)->getSpacing() == 2);
        REQUIRE_NOTHROW ( (*cellptr)->print() );

        cellptr = t.getCell('C', 2);
        REQUIRE ( (*cellptr)->getType() == Type::STRING );
        REQUIRE ( (*cellptr)->getNum_Value() == 100);
        REQUIRE ( (*cellptr)->getS_Value() == "\"100\"");
        REQUIRE ( (*cellptr)->getSpacing() == 3);

        cellptr = t.getCell('C', 1);
        REQUIRE ( (*cellptr)->getType() == Type::STRING );
        REQUIRE ( (*cellptr)->getNum_Value() == 0);
        REQUIRE ( (*cellptr)->getS_Value() == "\"Hey\"");
        REQUIRE ( (*cellptr)->getSpacing() == 3);

        cellptr = t.getCell('C', 60);
        REQUIRE (cellptr == nullptr);
    }


    SECTION ("File constructor")
    {
        std::ofstream write("test.csv", std::ios::trunc);
        if (!write.is_open()){
            throw std::runtime_error("Error loading file");
        }
        write << "20,\"Yes\"\n";
        write << ", 42";
        write.close();

        std::ifstream read("test.csv");
        Table t(read);
        // 20 | Yes
        //    |  42

        REQUIRE_NOTHROW (t.print());

        Cell** cellptr = t.getCell('A', 1);
        REQUIRE ( (*cellptr)->getType() == Type::INT );
        REQUIRE ( (*cellptr)->getNum_Value() == 20);
        REQUIRE ( (*cellptr)->getS_Value() == "20");
        REQUIRE ( (*cellptr)->getSpacing() == 2);

        cellptr = t.getCell('A', 2);
        REQUIRE ( (*cellptr)->getType() == Type::EMPTY );
        REQUIRE ( (*cellptr)->getNum_Value() == 0);
        REQUIRE ( (*cellptr)->getS_Value() == "");
        REQUIRE ( (*cellptr)->getSpacing() == 0);

        cellptr = t.getCell('C', 1);
        REQUIRE (cellptr == nullptr);
    }


    SECTION ("Setting new values")
    {
        std::string row1("=B1*C2, 0.8, 123");
        std::string row2(",220,\"100\"");
        // 80 | 0.8 | 123
        //    | 220 | 100
        Table t;
        t.addRow(row1);
        t.addRow(row2);

        std::string newValue("123.123.123");
        REQUIRE_THROWS (t.setValue('A',1,newValue));

        newValue = "Hello";
        REQUIRE_THROWS (t.setValue('A',1,newValue));

        newValue = "\"Hello\"";
        REQUIRE_NOTHROW (t.setValue('A',1,newValue));
        
        Cell** cellptr = t.getCell('A', 1);
        REQUIRE ( (*cellptr)->getType() == Type::STRING );
        REQUIRE ( (*cellptr)->getNum_Value() == 0);
        REQUIRE ( (*cellptr)->getS_Value() == "\"Hello\"");
        REQUIRE ( (*cellptr)->getSpacing() == 5);

        newValue = "=B2+C2";
        REQUIRE_NOTHROW (t.setValue('D',5,newValue));
        // 80 | 0.8 | 123 |     |
        //    | 220 | 100 |     |
        //    |     |     |     |      
        //    |     |     |     |
        //    |     |     | 320 |

        cellptr = t.getCell('D', 5); 
        REQUIRE ( (*cellptr)->getType() == Type::FORMULA );
        REQUIRE ( (*cellptr)->getNum_Value() == 320);
        REQUIRE ( (*cellptr)->getS_Value() == "=B2+C2");
        REQUIRE ( (*cellptr)->getSpacing() == 3);

        t.align(); //we have to align otherwise, D3 wont exist
        cellptr = t.getCell('D', 3); 
        REQUIRE ( (*cellptr)->getType() == Type::EMPTY );
        REQUIRE ( (*cellptr)->getNum_Value() == 0);
        REQUIRE ( (*cellptr)->getS_Value() == "");
        REQUIRE ( (*cellptr)->getSpacing() == 0); 
    }


    SECTION ("Printing and aligning")
    {
        std::string row1("=B1*C2, 0.8, 123");
        std::string row2(",220,\"100\"");
        // 80 | 0.8 | 123
        //    | 220 | 100
        Table t;
        t.addRow(row1);
        t.addRow(row2);

        std::cout << "\n--------------------------------\n";
        t.print(); //you can see it is aligned
        std::cout << "\n--------------------------------\n";

        std::string newValue("\"qwertyuiop\"");
        t.setValue('A', 2, newValue);
        t.print(); //you can see it is aligned
    }


    SECTION ("Saving in file")
    {
        std::string row1("=B1*C2, 0.8, 123");
        std::string row2(",220,\"100\"");
        // 80 | 0.8 | 123
        //    | 220 | 100
        Table t;
        t.addRow(row1);
        t.addRow(row2);

        std::ofstream write("saved.csv", std::ios::trunc);
        if (!write.is_open()){
            throw std::runtime_error("Error loading file");
        }
        t.saveInFile(write);
        write.close();

        std::ifstream read("saved.csv");
        if (!read.is_open()){
            throw std::runtime_error("Error loading file");
        }

        std::string rowValue;
        getline (read, rowValue);
        REQUIRE (rowValue == "=B1*C2,0.8,123");

        rowValue.clear();
        getline (read, rowValue);
        REQUIRE (rowValue == ",220,\"100\"");

        REQUIRE (read.eof());
        
        read.close();
    }
}



TEST_CASE ("Testing commands")
{
    SECTION ("Constructor")
    {
        Commands cmds;
        REQUIRE (cmds.getPath().size() == 0);
        REQUIRE (!cmds.isThereTable());
    }


    SECTION ("Testing with no document created/opened")
    {
        Commands cmds;
        REQUIRE_THROWS (cmds.SAVE());
        REQUIRE_THROWS (cmds.SAVEAS("sth.csv"));
        REQUIRE_THROWS (cmds.GET("A12"));
        std::string address("A12");
        std::string value("123");
        REQUIRE_THROWS (cmds.EDIT(address,value));
        REQUIRE_THROWS (cmds.PRINT());
    }


    SECTION ("Testing readCellAddress via GET()")
    {
        std::ofstream write("test.csv", std::ios::trunc);
        if (!write.is_open()){
            throw std::runtime_error("Error loading file");
        }
        write << "20,\"Yes\"\n";
        write << ", 42";
        write.close();

        Commands cmds;
        cmds.OPEN("test.csv");

        REQUIRE_THROWS (cmds.GET("A"));
        REQUIRE_THROWS (cmds.GET("1A"));
        REQUIRE_THROWS (cmds.GET("AA1"));
        REQUIRE_THROWS (cmds.GET("A0"));
        REQUIRE_THROWS (cmds.GET("A-1"));

        REQUIRE_NOTHROW (cmds.GET("A1"));
        REQUIRE_NOTHROW (cmds.GET("A0001"));
        REQUIRE_NOTHROW (cmds.GET("B2"));
        REQUIRE_NOTHROW (cmds.GET("C1000"));
    }


    SECTION ("Working with created document")
    {
        Commands cmds;
        cmds.NEW();
        REQUIRE (cmds.isThereTable());

        //doing some things
        std::string newValue("123");
        cmds.EDIT("C3", newValue);
        newValue = "\"radoo\"";
        cmds.EDIT("A4", newValue);
        cmds.PRINT();

        REQUIRE_THROWS (cmds.SAVE());
        cmds.SAVEAS("yoho.csv");
        cmds.CLOSE();
        REQUIRE (!cmds.isThereTable());
        REQUIRE (cmds.getPath().size() == 0);
    }


    SECTION ("Working with opened document")
    {
        std::ofstream write("test.csv", std::ios::trunc);
        if (!write.is_open()){
            throw std::runtime_error("Error loading file");
        }
        write << "20,\"Yes\"\n";
        write << ", 42";
        write.close();
        // 20 | Yes
        //    |  42

        Commands cmds;
        cmds.OPEN("test.csv");

        std::string newValue("0.07");
        cmds.EDIT("A1", newValue);

        REQUIRE_NOTHROW (cmds.SAVE());
        cmds.CLOSE();
        REQUIRE (!cmds.isThereTable());
        REQUIRE (cmds.getPath().size() == 0);
    }
}



TEST_CASE ("Testing Program")
{
    SECTION ("Invalid commands")
    {
        std::ofstream write("test.csv", std::ios::trunc);
        if (!write.is_open()){
            throw std::runtime_error("Error loading file");
        }
        write << "20,\"Yes\"\n";
        write << ", 42";
        write.close();
        // 20 | Yes
        //    |  42

        Program p;

        p.executeCommand("open test.csv");
        REQUIRE_THROWS (p.executeCommand("save arg"));
        REQUIRE_THROWS (p.executeCommand("+-alsjnd"));
        REQUIRE_THROWS (p.executeCommand("yes.csv saveas"));
        REQUIRE_THROWS (p.executeCommand(""));
        REQUIRE_THROWS (p.executeCommand("get AA1"));
        REQUIRE_THROWS (p.executeCommand("get A 1"));
        REQUIRE_THROWS (p.executeCommand("edit a 1 123"));
        REQUIRE_THROWS (p.executeCommand("edit A0 123"));
        REQUIRE_THROWS (p.executeCommand("edit a-1 123"));
        REQUIRE_THROWS (p.executeCommand("exit x"));
    }


    SECTION ("Valid commands")
    {
        std::ofstream write("test.csv", std::ios::trunc);
        if (!write.is_open()){
            throw std::runtime_error("Error loading file");
        }
        write << "20,\"Yes\"\n";
        write << ", 42";
        write.close();
        // 20 | Yes
        //    |  42

        Program p;

        p.executeCommand("open test.csv");
        REQUIRE_NOTHROW (p.executeCommand("get A1"));
        REQUIRE_NOTHROW (p.executeCommand("GET A1"));
        REQUIRE_NOTHROW (p.executeCommand("geT a1"));
        REQUIRE_NOTHROW (p.executeCommand("   geT      a1"));
        REQUIRE_NOTHROW (p.executeCommand("edit a1 123"));
        REQUIRE_NOTHROW (p.executeCommand("edit a1 =  13 * 52"));
        REQUIRE_NOTHROW (p.executeCommand("edit a1     = A2*5"));
        REQUIRE_NOTHROW (p.executeCommand("edit a1 =A1*4"));
    }   
}