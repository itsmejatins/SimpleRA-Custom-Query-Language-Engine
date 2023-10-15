//
// Created by System Administrator on 01/09/23.
//

#include "global.h"
#include <sstream>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <cstdio>

/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD MATRIX command has been called. This command should be
 * followed by calling the load matrix function;
 *
 * @param matrixName
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD MATRIX command has been called. This command should be
 * followed by calling the load matrix function;
 *
 * @param fromMatrixName
 * @param toMatrixName
 */
Matrix::Matrix(string fromMatrixName, string toMatrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + fromMatrixName + ".csv";
    this->matrixName = toMatrixName;
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates matrix
 * statistics.
 *
 * @return true if the matrix has been successfully loaded
 * @return false if an error occurred
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        unordered_set<string> columnValues;
        string word;
        stringstream s(line);

        while (getline(s, word, ','))
        {
            word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
            columnValues.insert(word);
            this->columns.emplace_back(word);
        }

        this->columnCount = this->columns.size();
        this->maxRowsPerBlock = (unsigned int)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));

        fin.close();

        if (this->blockify())
            return true;
    }

    fin.close();
    return false;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->matrixName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->matrixName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row
 */
void Matrix::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Renames the matrix indicated by fromMatrixName to toMatrixName. It is
 * assumed that checks such as the existence of fromMatrixName and the non prior
 * existence of toMatrixName are done.
 *
 * @param fromMatrixName
 * @param toMatrixName
 */
void Matrix::renameMatrix(string fromMatrixName, string toMatrixName)
{
    return;
}

/**
 * @brief Function prints the first few rows of the matrix. If the matrix contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::print()
{
    logger.log("Matrix::print");
    unsigned int count = min((long long)PRINT_COUNT, this->rowCount);
    cout << endl ;

    // print headings
    //  this->writeRow(this->columns, cout);

    Cursor cursor(this->matrixName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext("matrix");
        // considering only the count number of columns
        vector<int> finalRow;
        for (int i = 0; i < count; i++)
        {
            finalRow.push_back(row[i]);
        }

        this->writeRow(finalRow, cout);
    }
    // printRowCount(this->rowCount);
}

/**
 * @brief This function returns one row of the matrix using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor
 * @return vector<int>
 */
void Matrix::getNextPage(Cursor *cursor)
{
    logger.log("Matrix::getNext");

    if (cursor->pageIndex < this->blockCount - 1)
    {
        cursor->nextPage(cursor->pageIndex + 1);
    }
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    // this->writeRow(this->columns, fout);

    Cursor cursor(this->matrixName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext("matrix");
        this->writeRow(row, fout);
    }
    fout.close();
}
/**
 * @brief Function to check if matrix is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

void sleepUtility(int sec)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(sec));
}

vector<vector<int>> readPage(string matrixName, int pageNo, int &nReads, int &nWrites)
{
    string PATH = "../data/temp/";
    string fileName = PATH + matrixName + "_Page" + to_string(pageNo);
    ifstream infile(fileName);
    vector<vector<int>> data;
    string line;

    while (getline(infile, line))
    {
        vector<int> row;
        stringstream lineStream(line);
        int value;

        while (lineStream >> value)
        {
            row.push_back(value);
        }

        data.push_back(row);
    }
    nReads++;
    infile.close();
    return data;
}

void writeVectorToFile(vector<int> vec, string fileName, int &nReads, int &nWrites)
{
    ofstream outfile(fileName);
    for (size_t i = 0; i < vec.size(); ++i)
    {
        outfile << vec[i];
        if (i < vec.size() - 1)
        {
            outfile << " ";
        }
    }
    outfile.close();
    nWrites++;
}

vector<int> getRow(string matrixName, int rowPage, int rowIndex, int &nReads, int &nWrites)
{
    vector<vector<int>> data = readPage(matrixName, rowPage, nReads, nWrites);
    return data[rowIndex];
}

vector<int> getCol(string matrixName, int colIndex, int n_pages, int &nReads, int &nWrites)
{
    vector<int> col;
    for (int pageNo = 0; pageNo < n_pages; pageNo++)
    {
        vector<vector<int>> page = readPage(matrixName, pageNo, nReads, nWrites);
        for (vector<int> row : page)
        {
            col.push_back(row[colIndex]);
        }
    }
    return col;
}

void writeRowToTemporaryBlock(string matrixName, int pageNo, int rowIndex, int &nReads, int &nWrites)
{
    string tempFileName = "../data/temp/temp_block_row";
    vector<int> row = getRow(matrixName, pageNo, rowIndex, nReads, nWrites);
    writeVectorToFile(row, tempFileName, nReads, nWrites);
}

void writeColToTemporaryBlock(string matrixName, int colIndex, int n_pages, int &nReads, int &nWrites)
{
    string tempFileName = "../data/temp/temp_block_col";
    vector<int> col = getCol(matrixName, colIndex, n_pages, nReads, nWrites);
    writeVectorToFile(col, tempFileName, nReads, nWrites);
}

vector<int> readFromTempBlock(string fileName, int &nReads, int &nWrites)
{
    vector<int> numbers;
    ifstream infile(fileName);
    int number;
    while (infile >> number)
    {
        numbers.push_back(number);
    }
    infile.close();
    nReads++;
    return numbers;
}

void updatePage(vector<vector<int>> page, string matrixName, int pageNo, int &nReads, int &nWrites)
{
    string fileName = "../data/temp/" + matrixName + "_Page" + to_string(pageNo);
    ofstream outfile(fileName);

    for (const auto &row : page)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            outfile << row[i];
            if (i < row.size() - 1)
            {
                outfile << " ";
            }
        }
        outfile << endl;
    }
    outfile.close();
    nWrites++;
}

void updateRow(string matrixName, int rowPage, int rowIndex, int start, int &nReads, int &nWrites)
{
    // read the whole page
    vector<vector<int>> page = readPage(matrixName, rowPage, nReads, nWrites);

    // read vector from the block
    vector<int> newRow = readFromTempBlock("../data/temp/temp_block_col", nReads, nWrites);
    // update the rowIndex-th row

    for (int i = start; i < page[rowIndex].size(); i++)
    {
        page[rowIndex][i] = newRow[i];
    }
    // write that row to that page
    updatePage(page, matrixName, rowPage, nReads, nWrites);
}

void updateCol(string matrixName, int colIndex, int n_pages, int maxRow, int &nReads, int &nWrites)
{

    // reading from the temporary row
    vector<int> newCol = readFromTempBlock("../data/temp/temp_block_row", nReads, nWrites);

    int newColIter = colIndex;
    for (int pageNo = colIndex / maxRow; pageNo < n_pages; pageNo++)
    {
        vector<vector<int>> page = readPage(matrixName, pageNo, nReads, nWrites);
        int i = pageNo == colIndex / maxRow ? colIndex % maxRow : 0;
        for (; i < page.size(); i++)
        {
            page[i][colIndex] = newCol[newColIter++];
        }
        updatePage(page, matrixName, pageNo, nReads, nWrites);
    }
}

// Transpose Matrix
void Matrix::transposeMatrix()
{
    string matrixName = this->matrixName;
    int rows = this->rowCount;
    int rowPage = 0;
    int rowIndex = 0;
    int nReads = 0;
    int nWrites = 0;
    for (int i = 0; i < rows; i++)
    {
        rowPage = i / this->maxRowsPerBlock;
        rowIndex = i % this->maxRowsPerBlock;
        writeRowToTemporaryBlock(matrixName, rowPage, rowIndex, nReads, nWrites);
        writeColToTemporaryBlock(matrixName, i, this->blockCount, nReads, nWrites);

        updateRow(matrixName, rowPage, rowIndex, i, nReads, nWrites);
        updateCol(matrixName, i, this->blockCount, this->maxRowsPerBlock, nReads, nWrites);
    }
    cout << endl;
    cout << "Number of blocks read: " << nReads << endl;
    cout << "Number of blocks written: " << nWrites << endl;
    cout << "Number of blocks accessed: " << nReads + nWrites << endl;
    bufferManager.clearPool();
}

// helper function for checking Symmetry
bool checkTempBlocks(int &nReads, int &nWrites)
{
    vector<int> newRow = readFromTempBlock("../data/temp/temp_block_col", nReads, nWrites);
    vector<int> newCol = readFromTempBlock("../data/temp/temp_block_row", nReads, nWrites);

    int size = newRow.size();
    for (int i = 0; i < size; i++)
    {
        if (newRow[i] != newCol[i])
            return false;
    }
    return true;
}

bool Matrix::checkSymmetry()
{
    cout << "check symmetry " << endl;
    cout << this->matrixName << endl;

    int rowPage = 0;
    int rowIndex = 0;
    int nReads = 0;
    int nWrites = 0;
    bool symmetry = true;
    for (int i = 0; i < this->rowCount; i++)
    {
        rowPage = i / this->maxRowsPerBlock;
        rowIndex = i % this->maxRowsPerBlock;
        writeRowToTemporaryBlock(matrixName, rowPage, rowIndex, nReads, nWrites);
        writeColToTemporaryBlock(matrixName, i, this->blockCount, nReads, nWrites);
        if (checkTempBlocks(nReads, nWrites) == false){
            symmetry = false;
            break;
        }
            
    }

    cout << endl;
    cout << "Number of blocks read: " << nReads << endl;
    cout << "Number of blocks written: " << nWrites << endl;
    cout << "Number of blocks accessed: " << nReads + nWrites << endl;

    return symmetry;
}

// Function to create a copy of the file
bool createCopy(const std::string& sourceFilePath, const std::string& destinationFilePath) {
    // Open the source file for reading
    std::ifstream sourceFile(sourceFilePath, std::ios::binary);

    if (!sourceFile.is_open()) {
        std::cerr << "Error: Unable to open the source file." << std::endl;
        return false;
    }

    // Open the destination file for writing
    std::ofstream destinationFile(destinationFilePath, std::ios::binary);

    if (!destinationFile.is_open()) {
        std::cerr << "Error: Unable to create the destination file." << std::endl;
        return false;
    }

    // Copy the contents of the source file to the destination file
    destinationFile << sourceFile.rdbuf();

    // Close both files
    sourceFile.close();
    destinationFile.close();

    return true;
}

bool deleteFile(const std::string& destinationFilePath) {
    // Attempt to remove the destination file
    if (std::remove(destinationFilePath.c_str()) != 0) {
        std::cerr << "Error: Unable to delete the file." << std::endl;
        return false;
    }

    return true;
}

bool renameFile(const std::string& currentFilePath, const std::string& newFilePath) {
    // Attempt to rename the file
    if (std::rename(currentFilePath.c_str(), newFilePath.c_str()) != 0) {
        std::cerr << "Error: Unable to rename the file." << std::endl;
        return false;
    }

    return true;
}

void Matrix::computeMatrix()
{
    int nReads = 0, nWrites = 0;
    string srcPath = "../data/" + this->matrixName + ".csv";
    string desPath = "../data/" + this->matrixName + "_RESULT.csv";

    bool origFileExists = isFileExistsAnywhere(srcPath);

    if(origFileExists)
    {
        createCopy(srcPath, "../data/" + this->matrixName + "_ORIG.csv");
        nReads+=this->blockCount;
        nWrites+=this->blockCount;
    }
    this->makePermanent();
    nWrites += this->blockCount;
    createCopy(srcPath, desPath);
    nReads+=this->blockCount;
    nWrites+=this->blockCount;

    Matrix *matrix = new Matrix(this->matrixName + "_RESULT");

    if (matrix->load())
    {
        if(matrix->columnCount != matrix->rowCount)
        {
            cout << "It's not a square matrix!";
            return;
        }

        matrixCatalogue.insertMatrix(matrix);

        //////////////////////////////////////////////


        string matrixName = matrix->matrixName;
        int rows = matrix->rowCount;
        int rowPage = 0;
        int rowIndex = 0;
        for (int i = 0; i < rows; i++)
        {
            rowPage = i / matrix->maxRowsPerBlock;
            rowIndex = i % matrix->maxRowsPerBlock;
            writeRowToTemporaryBlock(matrixName, rowPage, rowIndex, nReads, nWrites);
            writeColToTemporaryBlock(matrixName, i, matrix->blockCount, nReads, nWrites);

            updateRow(matrixName, rowPage, rowIndex, i, nReads, nWrites);
            updateCol(matrixName, i, matrix->blockCount, matrix->maxRowsPerBlock, nReads, nWrites);
        }
        bufferManager.clearPool();

        ///////////////////////////////////////////


    }

    deleteFile(desPath);
    deleteFile(srcPath);
    if(origFileExists)
        renameFile("../data/" + this->matrixName + "_ORIG.csv", srcPath);

    for(int p = 0; p < this->blockCount; p++)
    {
        auto v1 = readPage(this->matrixName, p, nReads, nWrites);
        auto v2 = readPage(this->matrixName+"_RESULT", p, nReads, nWrites);

        for(int i = 0; i < v2.size(); i++)
        {
            for(int j = 0; j < v2.at(0).size(); j++)
            {
                v2[i][j] = v1[i][j] - v2[i][j];
            }
        }
        updatePage(v2, this->matrixName + "_RESULT", p, nReads, nWrites);
    }

    cout << endl;
    cout << "Number of blocks read: " << nReads << endl;
    cout << "Number of blocks written: " << nWrites << endl;
    cout << "Number of blocks accessed: " << nReads + nWrites << endl;
}


/**
 * @brief The unload function removes the matrix from the database by deleting
 * all temporary files created as part of this matrix
 *
 */
void Matrix::unload()
{
    logger.log("Matrix::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->matrixName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this matrix
 *
 * @return Cursor
 */
Cursor Matrix::getCursor()
{
    logger.log("Matrix::getCursor");
    Cursor cursor(this->matrixName, 0);
    return cursor;
}