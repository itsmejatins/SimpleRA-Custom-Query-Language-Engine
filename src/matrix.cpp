//
// Created by System Administrator on 01/09/23.
//

#include "global.h"
#include <sstream>
#include <chrono>
#include <thread>

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

    // print headings
    //  this->writeRow(this->columns, cout);

    Cursor cursor(this->matrixName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext("matrix");
        //considering only the count number of columns
        vector<int> finalRow;
        for(int i=0;i<count;i++){
            finalRow.push_back(row[i]);
        }

        this->writeRow(finalRow, cout);
    }
    printRowCount(this->rowCount);
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
    if (!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    // print headings
    //  this->writeRow(this->columns, fout);

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

void sleepUtility(int sec) {
    std::this_thread::sleep_for(std::chrono::milliseconds(sec));
}

vector<vector<int>> readPage(string matrixName, int pageNo)
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

    infile.close();
    return data;
}

void writeVectorToFile(vector<int> vec, string fileName)
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
}

vector<int> getRow(string matrixName, int rowPage, int rowIndex)
{
    vector<vector<int>> data = readPage(matrixName, rowPage);
    return data[rowIndex];
}

vector<int> getCol(string matrixName, int colIndex,int n_pages)
{
    vector<int> col;
    for(int pageNo=0; pageNo < n_pages; pageNo++){
        vector<vector<int>> page = readPage(matrixName,pageNo);
        for(vector<int> row : page ){
            col.push_back(row[colIndex]);
        }
    }
    return col;
}

void writeRowToTemporaryBlock(string matrixName, int pageNo, int rowIndex)
{
    string tempFileName = "../data/temp/temp_block_row";
    vector<int> row = getRow(matrixName,pageNo,rowIndex);
    writeVectorToFile(row,tempFileName);
}

void writeColToTemporaryBlock(string matrixName, int colIndex,int n_pages){
    string tempFileName = "../data/temp/temp_block_col";
    vector<int> col = getCol(matrixName,colIndex,n_pages);
    writeVectorToFile(col,tempFileName);
}

vector<int> readFromTempBlock(string fileName){
    vector<int> numbers;
    ifstream infile(fileName);
    int number;
    while (infile >> number) {
        numbers.push_back(number);
    }
    infile.close();
    return numbers;
}

void updatePage(vector<vector<int>> page, string matrixName, int pageNo){
    string fileName = "../data/temp/"+matrixName+"_Page"+ to_string(pageNo);
    ofstream outfile(fileName);

    for (const auto& row : page) {
        for (size_t i = 0; i < row.size(); ++i) {
            outfile << row[i];
            if (i < row.size() - 1) {
                outfile << " ";
            }
        }
        outfile << endl;
    }
    outfile.close();
}

void updateRow(string matrixName, int rowPage, int rowIndex,int start){
    // read the whole page
    vector<vector<int>> page = readPage(matrixName,rowPage);

    //read vector from the block -
    vector<int> newRow = readFromTempBlock("../data/temp/temp_block_col");
    // update the rowIndex-th row

    for(int i = start ; i < page[rowIndex].size(); i++){
        page[rowIndex][i] = newRow[i];
    }
    // write that row to that page
    updatePage(page,matrixName,rowPage);
}

void updateCol(string matrixName, int colIndex, int n_pages,int maxRow){

    //reading from the temporary row
    vector<int> newCol = readFromTempBlock("../data/temp/temp_block_row");

    int newColIter=colIndex;
    for(int pageNo = colIndex/maxRow; pageNo < n_pages; pageNo++) {
        vector<vector<int>> page = readPage(matrixName, pageNo);
        int i = pageNo == colIndex/maxRow ? colIndex % maxRow : 0;
        for (; i < page.size(); i++) {
            page[i][colIndex] = newCol[newColIter++];
        }
        updatePage(page, matrixName, pageNo);
    }
}

void Matrix::transposeMatrix() {
//    cout << "TRANSPOSE MATRIX ";
    string matrixName = this->matrixName;
    int rows = this->rowCount;
    int rowPage = 0;
    int rowIndex = 0;
    int read=0;
    int write=0;
    for (int i = 0; i < rows; i++)
    {
        rowPage = i / this->maxRowsPerBlock;
        rowIndex = i % this->maxRowsPerBlock;
        writeRowToTemporaryBlock(matrixName, rowPage, rowIndex);
        read++; //reading row from the block
        write++; //writing the row to a temporary block
        writeColToTemporaryBlock(matrixName, i,this->blockCount);
        read++; //reading the column from the block
        write++; //writing the column to a temporary block

        updateRow(matrixName,rowPage,rowIndex,i);
        write++;    //writing the row to the original block
        updateCol(matrixName,i,this->blockCount,this->maxRowsPerBlock);
        write++;    //writing the column to the original column
    }
    cout << endl ;
    cout << "Number of blocks read: " << read << endl;
    cout << "Number of blocks written: "<< write << endl;
    cout << "Number of blocks accessed: " << read+write << endl;
    bufferManager.clearPool();

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