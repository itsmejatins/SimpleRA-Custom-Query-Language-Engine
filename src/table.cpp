#include "global.h"
#include <sstream>
#include <iostream>
#include <cstring>
#include <algorithm>
/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName
 * @param columns
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (unsigned int)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded
 * @return false if an error occurred
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file.
 *
 * @param line
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    int attributeCount=0;
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->attributeIndexMap.insert({word,attributeCount});
        attributeCount++;
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (unsigned int)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
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
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
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
void Table::updateStatistics(vector<int> row)
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
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName
 * @return true
 * @return false
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName
 * @param toColumnName
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    unsigned int count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext("table");
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}

/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor
 * @return vector<int>
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

    if (cursor->pageIndex < this->blockCount - 1)
    {
        cursor->nextPage(cursor->pageIndex+1);
    }
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext("table");
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
        return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 *
 * @return Cursor
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 *
 * @param columnName
 * @return int
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}

vector<vector<int> > readPage(string filePath)
{
    vector<vector<int>> data;
    ifstream infile(filePath);
    if(!infile)
    {
        cout << "Error in reading the file" << filePath << endl;
        return data;
    }
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

        if(row.size() > 0)
            data.push_back(row);
    }
    infile.close();
    return data;
}


void writeVectorToFile(vector<int> vec, string fileName)
{
    if(vec.size() == 0)
        return;

    ofstream outfile(fileName, ios::app);
    if(!outfile)
    {
        cout << "Error in writing to the file, returing" << endl;
        return;
    }
    for (size_t i = 0; i < vec.size(); ++i)
    {
        outfile << vec[i];
        if (i < vec.size() - 1)
        {
            outfile << " ";
        }
    }
    outfile << '\n';
    outfile.close();
}

vector<int> sortColIndices ;

bool compareTwoVectors(const vector<int>& a, const vector<int>& b) {

    for(int c = 0 ; c < sortColIndices.size(); c++)
    {
        int col = sortColIndices.at(c);
        if(a[col] == b[col])
            continue;
        string strategy = parsedQuery.sortingStrategies.at(c);
        if(strategy == "ASC")
            return a.at(col) < b.at(col);
        else
            return a.at(col) > b.at(col);
    }

    return a[0] < b[0];
}

void Table::initialRun()
{
    logger.log("Table::initialRun");

    for(string s : parsedQuery.sortColumnNames)
    {
        sortColIndices.push_back(this->attributeIndexMap[s]);
    }

    for(int pageNumber = 0; pageNumber < this->blockCount; pageNumber++)
    {
        string filePath = "../data/temp/" + this->tableName + "_Page" + to_string(pageNumber);
        vector<vector<int> > currPage = readPage(filePath);
        std::sort(currPage.begin(), currPage.end(), compareTwoVectors);
        ofstream outfile(filePath, ios::trunc);
        if(!outfile)
        {
            cout << "Error in writing to the file, returing" << endl;
            return;
        }
        for(int r = 0; r < currPage.size(); r++)
        {
            for (size_t i = 0; i < currPage[r].size(); ++i)
            {
                outfile << currPage[r][i];
                if (i < currPage[r].size() - 1)
                {
                    outfile << " ";
                }
            }
            outfile << '\n';
        }
        outfile.close();
    }
}




void Table::sort()
{
    logger.log("Table::sort");
    initialRun();
    int b = this->blockCount - 1; // if there are five blocks of the table, put b = 4,( 0 based indexing)
    string tableName = this->tableName;
    int maxRowsInBlock = this->maxRowsPerBlock;
    int writePageIndex = 0, writeRowIndex =0;
    string lastReadFlag = "";
    int jump = 1;

    for(int levelCount = 0; levelCount < ceil(log2(b + 1)); )
    {
        int i = 0;
        while(i <= b)
        {
            int iFinal = i + jump  - 1;
            int j = iFinal + 1, jFinal = min(j + jump - 1, b);
            if(j > b) // This means that i is in the range but there is no corresponding j. SO rename the remaining blocks according to the flag ($ or "")
            {
                for(int r = i; r <= b; r++)
                {
                    // rename rth block as per the flag
                    string oldFileName = "../data/temp/" + tableName + lastReadFlag + "_Page" + to_string(r);
                    string newFileName = "../data/temp/" + tableName + (lastReadFlag.length() == 0 ? "$" : "") + "_Page" + to_string(r);
                    rename(&oldFileName[0], &newFileName[0]);
                }
                break;
            }
            vector<vector<int> > iBlock = readPage("../data/temp/" + tableName + lastReadFlag + "_Page" + to_string(i));
            vector<vector<int> > jBlock = readPage("../data/temp/" + tableName + lastReadFlag + "_Page" + to_string(j));

            int ii = 0, jj = 0;

            while(true)
            {
                if(i == iFinal && j == jFinal && ii == iBlock.size() && jj == jBlock.size())
                {
                    // delete page i and j, whichever exists
                    string pathToCurrentBlock = "../data/temp/" + tableName + lastReadFlag + "_Page" + to_string(i);
                    remove(&pathToCurrentBlock[0]);
                    pathToCurrentBlock = "../data/temp/" + tableName + lastReadFlag + "_Page" + to_string(j);
                    remove(&pathToCurrentBlock[0]);

                    i = jFinal + 1;
                    break;
                }

                if(ii == iBlock.size() && i < iFinal)
                {
                    // delete page i
                    string pathToCurrentBlock = "../data/temp/" + tableName + lastReadFlag + "_Page" + to_string(i);
                    if(remove(&pathToCurrentBlock[0]) != 0)
                        cout << "Error in deleting file" << pathToCurrentBlock << endl;

                    // reset ii to 0 and update iBlock by the next i block
                    ii = 0;
                    i++;
                    iBlock = readPage("../data/temp/" + tableName + lastReadFlag + "_Page" + to_string(i));
                    continue;
                }
                if( jj == jBlock.size() && j < jFinal)
                {
                    // delete page j
                    string pathToCurrentBlock = "../data/temp/" + tableName + lastReadFlag + "_Page" + to_string(j);
                    if(remove(&pathToCurrentBlock[0]) != 0)
                        cout << "Error in deleting file" << pathToCurrentBlock << endl;

                    // reset jj to 0 and update jBlock by the next j block
                    jj = 0;
                    j++;
                    jBlock = readPage("../data/temp/" + tableName + lastReadFlag + "_Page" + to_string(j));
                    continue;
                }

                if(ii == iBlock.size() && i == iFinal)
                {
                    // copy remaining jBlocks, for now I am copying the next jBlock row till j == jFinal and j == jBlock.size() - 1
                    if(writeRowIndex + 1 > maxRowsInBlock)
                    {
                        writePageIndex++;
                        writeRowIndex = 0;
                    }
                    string writeFilePath = "../data/temp/" + tableName + (lastReadFlag.length() == 0 ? "$" : "") + "_Page" + to_string(writePageIndex);
                    writeVectorToFile(jBlock[jj], writeFilePath);
                    writeRowIndex++;
                    jj++;
                    continue;
                }
                if(jj == jBlock.size() && j == jFinal)
                {
                    // copy remaining iBlocks
                    if(writeRowIndex + 1 > maxRowsInBlock)
                    {
                        writePageIndex++;
                        writeRowIndex = 0;
                    }
                    string writeFilePath = "../data/temp/" +  tableName + (lastReadFlag.length() == 0 ? "$" : "") + "_Page" + to_string(writePageIndex);
                    writeVectorToFile(iBlock[ii], writeFilePath);
                    writeRowIndex++;
                    ii++;
                    continue;
//                    i = jFinal + 1;
//                    break;
                }


                // no edge case, copy the smaller number to the block
//                iBlock[ii][0] <= jBlock[jj][0]
                if(compareTwoVectors(iBlock[ii], jBlock[jj]))
                {
                    // copy iBlock[ii][0] to tempBlock
                    if(writeRowIndex + 1 > maxRowsInBlock)
                    {
                        writePageIndex++;
                        writeRowIndex = 0;
                    }
                    string writeFilePath = "../data/temp/" +  tableName + (lastReadFlag.length() == 0 ? "$" : "") + "_Page" + to_string(writePageIndex);
                    writeVectorToFile(iBlock[ii], writeFilePath);
                    writeRowIndex++;
                    ii++;
                    continue;
                }
                else
                {
                    // copy jBlock[jj][0] to tempBlock
                    if(writeRowIndex + 1 > maxRowsInBlock)
                    {
                        writePageIndex++;
                        writeRowIndex = 0;
                    }
                    string writeFilePath = "../data/temp/" + tableName + (lastReadFlag.length() == 0 ? "$" : "") + "_Page" + to_string(writePageIndex);
                    writeVectorToFile(jBlock[jj], writeFilePath);
                    writeRowIndex++;
                    jj++;
                    continue;
                }
            }
        }
        // one level completely done
        levelCount++;
        jump *= 2;
        lastReadFlag = lastReadFlag == "" ? "$" : "";
        writePageIndex = 0;
        writeRowIndex = 0;
    }

    if(lastReadFlag.length() > 0)
    {
        for(int i = 0; i < this->blockCount; i++)
        {
            string oldFilePath = "../data/temp/" + this->tableName + "$_Page" + to_string(i);
            string newFilePath = "../data/temp/" + this->tableName + "_Page" + to_string(i);
            rename(&oldFilePath[0], &newFilePath[0]);
        }
    }

    bufferManager.clearPool();
}