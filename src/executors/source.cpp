#include "global.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
/**
 * @brief 
 * SYNTAX: SOURCE filename
 */

bool syntacticParseSOURCE()
{
    logger.log("syntacticParseSOURCE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SOURCE;
    parsedQuery.sourceFileName = tokenizedQuery[1];
    return true;
}

bool semanticParseSOURCE()
{
    logger.log("semanticParseSOURCE");
    if (!isQueryFile(parsedQuery.sourceFileName))
    {
        cout << "SEMANTIC ERROR: File doesn't exist" << endl;
        cout << "file name " << parsedQuery.sourceFileName << endl;
        return false;
    }
    return true;
}

void executeSOURCE()
{
    logger.log("executeSOURCE");
    
    std::string filename = "../data/"+parsedQuery.sourceFileName+".ra"; // Change this to the path of your file
    std::ifstream inputFile(filename);
    // cout << filename << endl;
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return ;
    }

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }

    inputFile.close();

    regex delim("[^\\s,]+");
    for (const std::string& storedLine : lines) {

        cout << "\n> ";
        tokenizedQuery.clear();
        parsedQuery.clear();
        // logger.log("\nReading New Command: ");
        // getline(cin, command);
        
        string command = storedLine;
        logger.log(command);
        // cout << "source.cpp " << command << endl;

        auto words_begin = std::sregex_iterator(command.begin(), command.end(), delim);
        
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i)
            tokenizedQuery.emplace_back((*i).str());

        // if (tokenizedQuery.size() == 1 && tokenizedQuery.front() == "QUIT"){
        //     break;

        // if (tokenizedQuery.empty())
        //     continue;


        // if (tokenizedQuery.size() == 1)
        // {
        //     cout << "SYNTAX ERROR" << endl;
        //     continue;
        // }

        logger.log("doCommand");
        if (syntacticParse() && semanticParse()){
            executeCommand();
        }
    }
    return;
    }
    