//
// Created by System Administrator on 01/09/23.
//

#ifndef SRC_MATRIXCATALOGUE_H
#define SRC_MATRIXCATALOGUE_H



#include <unordered_map>
#include "matrix.h"
/**
 * @brief The MatrixCatalogue acts like an index of matrices existing in the
 * system. Everytime a matrix is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the matrixCatalogue.
 *
 */

class MatrixCatalogue
{

    unordered_map<string, Matrix*> matrices;

public:
    MatrixCatalogue() {}
    void insertMatrix(Matrix* matrix);
    void deleteMatrix(string matrixName);
    Matrix* getMatrix(string matrixName);
    bool isMatrix(string matrixName);
    void print();
    ~MatrixCatalogue();
};

#endif //SRC_MATRIXCATALOGUE_H
