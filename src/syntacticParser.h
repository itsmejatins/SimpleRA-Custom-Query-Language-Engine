#include "tableCatalogue.h"
#include "matrixCatalogue.h"

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    ORDER_BY,
    UNDETERMINED,
    GROUP,

    // for matrix
    CLEARMATRIX,
    LOADMATRIX,
    PRINTMATRIX,
    RENAMEMATRIX,
    EXPORTMATRIX,

    TRANSPOSEMATRIX,
    CHECKSYMMETRY,
    COMPUTEMATRIX

};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

enum AggregateFunction{
    MIN,
    MAX,
    SUM,
    COUNT,
    AVG
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";
    string transposeRelationName = "";
    string computeRelationName = "";
    string checkSymmetryRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    // for matrix
    string renameFromMatrixName = "";
    string renameToMatrixName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    vector<string> sortingStrategies;

    string sortResultRelationName = "";
    string sortColumnName = "";
    vector<string> sortColumnNames;
    string sortRelationName = "";

    string sourceFileName = "";


    string orderByRelation = "";
    string orderByResultRelation = "";
    string orderByCol = "";
    string orderByStrategy = "";

    //GROUP BY attributes

    //column in the table based on which table has to be sorted.
    string groupByResultRelationName = "";
    string groupingAttribute = "";
    string groupByRelationName = "";

    AggregateFunction groupByConditionAggregateFunction;
    string groupByConditionAttribute = "";

    //comparison operator mentioned above
    BinaryOperator groupByBinaryOperator = NO_BINOP_CLAUSE;

    unsigned int groupByAggregateThreshold;
    AggregateFunction groupByReturnAggregateFunction;
    string groupByReturnAttribute = "";


    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseGROUPBY();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();
bool syntacticParseORDERBY();

// for matrix
bool syntacticParseCLEARMATRIX();
bool syntacticParseLOADMATRIX();
bool syntacticParsePRINTMATRIX();
bool syntacticParseEXPORTMATRIX();
bool syntacticParseRENAMEMATRIX();

//transpose matrix
bool syntacticParseTRANSPOSEMATRIX();
bool syntacticParseCOMPUTEMATRIX();
bool syntacticParseCHECKSYMMETRY();

bool isFileExists(string relationName);
bool isFileExistsAnywhere(const std::string& filePath);
bool isQueryFile(string relationName);

//utilityFunctions
void printVectorString(vector<string> stringVec);
void printVectorInt(vector<int> intVec);