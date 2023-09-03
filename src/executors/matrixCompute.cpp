#include "global.h"

bool syntacticParseCOMPUTEMATRIX()
{
    logger.log("syntacticParseCOMPUTEMATRIX");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTEMATRIX;
    parsedQuery.computeRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCOMPUTEMATRIX()
{
    logger.log("semanticParseCOMPUTEMATRIX");

    if (!isFileExists(parsedQuery.computeRelationName))
    {
        if(isFileExistsAnywhere("../data/temp/" + parsedQuery.computeRelationName + "_Page0"))
            return true;
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCOMPUTEMATRIX(){
    logger.log("executeCOMPUTEMATRIX");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.computeRelationName);
    matrix->computeMatrix();

}