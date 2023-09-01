
enum MatrixIndexingStrategy
{
    MATRIX_BTREE,
    MATRIX_HASH,
    MATRIX_NOTHING
};

/**
 * @brief The Matrix class holds all information related to a loaded matrix. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a matrix object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use operations like (TRANSPOSE, SUBTRACT).
 *
 */
class Matrix
{
    vector<unordered_set<int>> distinctValuesInColumns;

public:
    string sourceFileName = "";
    string matrixName = "";
    vector<string> columns;
    vector<unsigned int> distinctValuesPerColumnCount;
    unsigned int columnCount = 0;
    long long int rowCount = 0;
    unsigned int blockCount = 0;
    unsigned int maxRowsPerBlock = 0;
    vector<unsigned int> rowsPerBlockCount;
    bool indexed = false;
    string indexedColumn = "";
    MatrixIndexingStrategy matrixIndexingStrategy = MATRIX_NOTHING;

    bool blockify();
    void updateStatistics(vector<int> row);
    Matrix();
    Matrix(string matrixName);
    Matrix(string fromMatrixName, string toMatrixName);
    Matrix(string matrixName, vector<string> columns);
    bool load();
    void renameMatrix(string fromMatrixName, string toMatrixName);
    void print();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    int getColumnIndex(string columnName);
    void unload();

    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row
 */
    template <typename T>
    void writeRow(vector<T> row, ostream &fout)
    {
        logger.log("Matrix::printRow");
        for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
        {
            if (columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        fout << endl;
    }

/**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row
 */
    template <typename T>
    void writeRow(vector<T> row)
    {
        logger.log("Matrix::printRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }
};
