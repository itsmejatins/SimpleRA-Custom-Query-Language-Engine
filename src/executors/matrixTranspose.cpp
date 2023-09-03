#include "global.h"
/**
 * @brief
 * SYNTAX: TRANSPOSE MATRIX relation_name
 */


bool syntacticParseTRANSPOSEMATRIX()
{
    logger.log("syntacticParseTRANSPOSEMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSEMATRIX;
    parsedQuery.transposeRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseTRANSPOSEMATRIX()
{
    logger.log("semanticParseTRANSPOSEMATRIX");

    if (!isFileExists(parsedQuery.transposeRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeTRANSPOSEMATRIX(){
    logger.log("executeTRANSPOSEMATRIX");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.transposeRelationName);
    matrix->transposeMatrix();

}