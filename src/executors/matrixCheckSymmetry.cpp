#include "global.h"
/**
 * @brief
 * SYNTAX: CHECKSYMMETRY relation_name
 */


bool syntacticParseCHECKSYMMETRY()
{
    logger.log("syntacticParseCHECKSYMMETRY");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.checkSymmetryRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCHECKSYMMETRY()
{
    logger.log("semanticParseCHECKSYMMETRY");
    if (!isFileExists(parsedQuery.checkSymmetryRelationName))
    {
        if(isFileExistsAnywhere("../data/temp/" + parsedQuery.checkSymmetryRelationName + "_Page0"))
            return true;

        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCHECKSYMMETRY(){
    logger.log("executeCHECKSYMMETRY");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.checkSymmetryRelationName);
    bool symmetry = matrix->checkSymmetry();
    string result = symmetry ? "TRUE" : "FALSE";
    cout << result << endl;

}