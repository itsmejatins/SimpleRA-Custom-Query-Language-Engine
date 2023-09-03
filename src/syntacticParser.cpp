#include "global.h"

bool syntacticParse()
{
    logger.log("syntacticParse");
    string possibleQueryType = tokenizedQuery[0];
    string possibleSubQueryType = tokenizedQuery[1];

    // for matrix
    if(possibleSubQueryType == "MATRIX")
    {
        if (possibleQueryType == "CLEAR")
            return syntacticParseCLEARMATRIX();

        else if (possibleQueryType == "LOAD")
            return syntacticParseLOADMATRIX();

        else if (possibleQueryType == "PRINT")
            return syntacticParsePRINTMATRIX();

        else if (possibleQueryType == "EXPORT")
            return syntacticParseEXPORTMATRIX();

        else if (possibleQueryType == "RENAME")
            return syntacticParseRENAMEMATRIX();

        else if(possibleQueryType == "TRANSPOSE")
            return syntacticParseTRANSPOSEMATRIX();
        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }

    if (tokenizedQuery.size() < 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if (possibleQueryType == "CLEAR")
        return syntacticParseCLEAR();
    else if (possibleQueryType == "INDEX")
        return syntacticParseINDEX();
    else if (possibleQueryType == "LIST")
        return syntacticParseLIST();
    else if (possibleQueryType == "LOAD")
        return syntacticParseLOAD();
    else if (possibleQueryType == "PRINT")
        return syntacticParsePRINT();
    else if (possibleQueryType == "RENAME")
        return syntacticParseRENAME();
    else if(possibleQueryType == "EXPORT")
        return syntacticParseEXPORT();
    else if(possibleQueryType == "SOURCE")
        return syntacticParseSOURCE();
    else if(possibleQueryType == "COMPUTE")
        return syntacticParseCOMPUTEMATRIX();
    else if( possibleQueryType == "CHECKSYMMETRY")
        return syntacticParseCHECKSYMMETRY();
    else
    {
        string resultantRelationName = possibleQueryType;
        if (tokenizedQuery[1] != "<-" || tokenizedQuery.size() < 3)
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        possibleQueryType = tokenizedQuery[2];
        if (possibleQueryType == "PROJECT")
            return syntacticParsePROJECTION();
        else if (possibleQueryType == "SELECT")
            return syntacticParseSELECTION();
        else if (possibleQueryType == "JOIN")
            return syntacticParseJOIN();
        else if (possibleQueryType == "CROSS")
            return syntacticParseCROSS();
        else if (possibleQueryType == "DISTINCT")
            return syntacticParseDISTINCT();
        else if (possibleQueryType == "SORT")
            return syntacticParseSORT();
        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
}

ParsedQuery::ParsedQuery()
{
}

void ParsedQuery::clear()
{
    logger.log("ParseQuery::clear");
    this->queryType = UNDETERMINED;

    this->clearRelationName = "";

    this->crossResultRelationName = "";
    this->crossFirstRelationName = "";
    this->crossSecondRelationName = "";

    this->distinctResultRelationName = "";
    this->distinctRelationName = "";

    this->exportRelationName = "";

    this->indexingStrategy = NOTHING;
    this->indexColumnName = "";
    this->indexRelationName = "";

    this->joinBinaryOperator = NO_BINOP_CLAUSE;
    this->joinResultRelationName = "";
    this->joinFirstRelationName = "";
    this->joinSecondRelationName = "";
    this->joinFirstColumnName = "";
    this->joinSecondColumnName = "";

    this->loadRelationName = "";

    this->printRelationName = "";

    this->projectionResultRelationName = "";
    this->projectionColumnList.clear();
    this->projectionRelationName = "";

    this->renameFromColumnName = "";
    this->renameToColumnName = "";
    this->renameRelationName = "";

    this->selectType = NO_SELECT_CLAUSE;
    this->selectionBinaryOperator = NO_BINOP_CLAUSE;
    this->selectionResultRelationName = "";
    this->selectionRelationName = "";
    this->selectionFirstColumnName = "";
    this->selectionSecondColumnName = "";
    this->selectionIntLiteral = 0;

    this->sortingStrategy = NO_SORT_CLAUSE;
    this->sortResultRelationName = "";
    this->sortColumnName = "";
    this->sortRelationName = "";

    this->sourceFileName = "";
}

/**
 * @brief Checks to see if source file exists. Called when LOAD command is
 * invoked.
 *
 * @param relationName
 * @return true
 * @return false
 */
bool isFileExists(string relationName)
{
    string fileName = "../data/" + relationName + ".csv";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

bool isFileExistsAnywhere(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

/**
 * @brief Checks to see if source file exists. Called when SOURCE command is
 * invoked.
 *
 * @param relationName
 * @return true
 * @return false
 */
bool isQueryFile(string relationName){
    relationName = "../data/" + relationName + ".ra";
    struct stat buffer;
    return (stat(relationName.c_str(), &buffer) == 0);
}