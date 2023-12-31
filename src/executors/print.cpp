#include "global.h"
/**
 * @brief
 * SYNTAX: PRINT relation_name
 */
bool syntacticParsePRINT()
{
    logger.log("syntacticParsePRINT");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINT;
    parsedQuery.printRelationName = tokenizedQuery[1];
    return true;
}

bool syntacticParsePRINTMATRIX()
{
    logger.log("syntacticParsePRINTMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINTMATRIX;
    parsedQuery.printRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParsePRINT()
{
    logger.log("semanticParsePRINTMATRIX");
    if (!tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

bool semanticParsePRINTMATRIX()
{
    logger.log("semanticParsePRINTMATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINT()
{
    logger.log("executePRINT");
    Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
    table->print();
}

void executePRINTMATRIX()
{
    logger.log("executePRINTMATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printRelationName);
    matrix->print();

    cout << endl;
    cout << "Number of blocks read: " << matrix->blockCount << endl;
    cout << "Number of blocks written: " << 0 << endl;
    cout << "Number of blocks accessed: " << matrix->blockCount << endl;
}