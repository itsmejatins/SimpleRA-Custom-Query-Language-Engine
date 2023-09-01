#include "global.h"
/**
 * @brief
 * SYNTAX: RENAME column_name TO column_name FROM relation_name
 * SYNTAX: RENAME MATRIX from_matrix_name to_matrix_name
 */
bool syntacticParseRENAME()
{
    logger.log("syntacticParseRENAME");
    if (tokenizedQuery.size() != 6 || tokenizedQuery[2] != "TO" || tokenizedQuery[4] != "FROM")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = RENAME;
    parsedQuery.renameFromColumnName = tokenizedQuery[1];
    parsedQuery.renameToColumnName = tokenizedQuery[3];
    parsedQuery.renameRelationName = tokenizedQuery[5];
    return true;
}

bool syntacticParseRENAMEMATRIX()
{
    logger.log("syntacticParseRENAMEMATRIX");
    if (tokenizedQuery.size() != 4)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = RENAMEMATRIX;
    parsedQuery.renameFromMatrixName = tokenizedQuery[2];
    parsedQuery.renameToMatrixName = tokenizedQuery[3];
    return true;
}

bool semanticParseRENAME()
{
    logger.log("semanticParseRENAME");

    if (!tableCatalogue.isTable(parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.renameFromColumnName, parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    if (tableCatalogue.isColumnFromTable(parsedQuery.renameToColumnName, parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Column with name already exists" << endl;
        return false;
    }
    return true;
}

bool semanticParseRENAMEMATRIX()
{
    logger.log("semanticParseRENAMEMATRIX");

    if (!matrixCatalogue.isMatrix(parsedQuery.renameFromMatrixName))
    {
        cout << "SEMANTIC ERROR: 'From Matrix' Relation doesn't exist" << endl;
        return false;
    }

    if (matrixCatalogue.isMatrix(parsedQuery.renameToMatrixName))
    {
        cout << "SEMANTIC ERROR: 'To Matrix' Relation already exist" << endl;
        return false;
    }

    return true;
}

void executeRENAME()
{
    logger.log("executeRENAME");
    Table* table = tableCatalogue.getTable(parsedQuery.renameRelationName);
    table->renameColumn(parsedQuery.renameFromColumnName, parsedQuery.renameToColumnName);
}

void executeRENAMEMATRIX()
{
    logger.log("executeRENAMEMATRIX");

    // delete old matrix
    matrixCatalogue.deleteMatrix(parsedQuery.renameFromMatrixName);

    // create new matrix
    Matrix *matrix = new Matrix(parsedQuery.renameFromMatrixName, parsedQuery.renameToMatrixName);

    if (matrix->load())
    {
        if(matrix->columnCount != matrix->rowCount)
        {
            cout << "It's not a square matrix!";
            return;
        }

        matrixCatalogue.insertMatrix(matrix);
    }
}