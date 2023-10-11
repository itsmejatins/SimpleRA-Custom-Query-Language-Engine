#include "global.h"

bool evaulateJOINCondition(const vector<int> &R_pointer, const vector<int> &S_pointer, int firstColIndex, int secondColIndex,
                      int binaryOperator);

/**
 * @brief 
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */


bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
//    printVectorInt(tokenizedQuery);
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "ON")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[3];
    parsedQuery.joinSecondRelationName = tokenizedQuery[4];
    parsedQuery.joinFirstColumnName = tokenizedQuery[6];
    parsedQuery.joinSecondColumnName = tokenizedQuery[8];

    string binaryOperator = tokenizedQuery[7];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

bool evaulateLESSTHANJOINCondition(const vector<int> &R_pointer, const vector<int> &S_pointer, int firstColIndex, int secondColIndex) {
    if( R_pointer.empty() || S_pointer.empty()) return false;
    if (parsedQuery.joinBinaryOperator == LESS_THAN) {
        return R_pointer[firstColIndex] >= S_pointer[secondColIndex];
    }else{
        return R_pointer[firstColIndex] > S_pointer[secondColIndex];
    }
}

bool evaulateGREATERTHANJOINCondition(const vector<int> &R_pointer, const vector<int> &S_pointer, int firstColIndex, int secondColIndex) {
    if (parsedQuery.joinBinaryOperator == LESS_THAN) {
        return R_pointer[firstColIndex] <= S_pointer[secondColIndex];
    }else{
        return R_pointer[firstColIndex] < S_pointer[secondColIndex];
    }
}

void performEQUIJOIN(Table table1,Table table2, vector<string> &columns){

    printVectorString(columns);

    Table *resultantTable = new Table(parsedQuery.joinResultRelationName, columns);

    Cursor cursor1 = table1.getCursor();
    Cursor cursor2 = table2.getCursor();

    vector<int> R_pointer = cursor1.getNext("table");
    vector<int> S_pointer = cursor2.getNext("table");
    vector<int> S_marker_row;

    string joinFirstColName = parsedQuery.joinFirstColumnName;
    string joinSecondColName = parsedQuery.joinSecondColumnName;

    int firstColIndex = table1.attributeIndexMap[joinFirstColName];
    int secondColIndex = table2.attributeIndexMap[joinSecondColName];

    vector<int> resultantRow;
    resultantRow.reserve(resultantTable->columnCount);

    while( !R_pointer.empty() ){
        while(!R_pointer.empty() && !S_pointer.empty() && R_pointer[firstColIndex] < S_pointer[secondColIndex] )   R_pointer = cursor1.getNext("table");
        while(!R_pointer.empty() && !S_pointer.empty() && R_pointer[firstColIndex] > S_pointer[secondColIndex] )   S_pointer = cursor2.getNext("table");

        if(!R_pointer.empty() && !S_pointer.empty() && R_pointer[firstColIndex] == S_pointer[secondColIndex] ){

            Cursor S_marker_cursor = cursor2;
            S_marker_row = S_pointer;

            while( !R_pointer.empty() && !S_pointer.empty() && R_pointer[firstColIndex] == S_pointer[secondColIndex] ){
                resultantRow = R_pointer;
                resultantRow.insert(resultantRow.end(),S_pointer.begin(),S_pointer.end());
                cout<< "res row " ;
                printVectorInt(resultantRow);
                resultantTable->writeRow(resultantRow);
                S_pointer = cursor2.getNext("table");
            }
            R_pointer = cursor1.getNext("table");
            S_pointer = S_marker_row;
            cursor2 = S_marker_cursor;
        }
    }
    resultantTable->blockify();
    tableCatalogue.insertTable(resultantTable);
    return;
}

void performLESSTHANCONDITIONALJOIN(Table table1,Table table2, vector<string> &columns){

    printVectorString(columns);

    Table *resultantTable = new Table(parsedQuery.joinResultRelationName, columns);

    Cursor cursor1 = table1.getCursor();
    Cursor cursor2 = table2.getCursor();

    vector<int> R_pointer = cursor1.getNext("table");
    vector<int> S_pointer;
    vector<int> S_marker;

    int firstColIndex = table1.attributeIndexMap[parsedQuery.joinFirstColumnName];
    int secondColIndex = table2.attributeIndexMap[parsedQuery.joinSecondColumnName];
    cout << parsedQuery.joinFirstColumnName << " " <<
        parsedQuery.joinSecondColumnName << " "<<
        firstColIndex << " " <<
        secondColIndex << " " << endl;

    vector<int> resultantRow;
    resultantRow.reserve(resultantTable->columnCount);

    while( !R_pointer.empty() ){
        cursor2 = table2.getCursor();
        S_pointer = cursor2.getNext("table");
        while(evaulateLESSTHANJOINCondition(R_pointer, S_pointer, firstColIndex, secondColIndex)) {
            S_pointer = cursor2.getNext(("table"));
        }
        while( !S_pointer.empty() ){
            resultantRow = R_pointer;
            resultantRow.insert(resultantRow.end(),S_pointer.begin(),S_pointer.end());
            cout<< "res row " ;
            printVectorInt(resultantRow);
            resultantTable->writeRow(resultantRow);
            S_pointer = cursor2.getNext("table");
        }
        R_pointer = cursor1.getNext(("table"));

    }
    resultantTable->blockify();
    tableCatalogue.insertTable(resultantTable);
    return;
}

void performGREATERTHANCONDITIONALJOIN(Table table1,Table table2, vector<string> &columns){

    printVectorString(columns);

    Table *resultantTable = new Table(parsedQuery.joinResultRelationName, columns);

    Cursor cursor1 = table1.getCursor();
    Cursor cursor2 = table2.getCursor();

    vector<int> R_pointer = cursor1.getNext("table");
    vector<int> S_pointer;
    vector<int> S_marker;

    int firstColIndex = table1.attributeIndexMap[parsedQuery.joinFirstColumnName];
    int secondColIndex = table2.attributeIndexMap[parsedQuery.joinSecondColumnName];
    cout << parsedQuery.joinFirstColumnName << " " <<
         parsedQuery.joinSecondColumnName << " "<<
         firstColIndex << " " <<
         secondColIndex << " " << endl;

    vector<int> resultantRow;
    resultantRow.reserve(resultantTable->columnCount);

    while( !R_pointer.empty() ){
        cursor2 = table2.getCursor();
        S_pointer = cursor2.getNext("table");
        while( !S_pointer.empty() ){
            if(evaulateLESSTHANJOINCondition(R_pointer,S_pointer,firstColIndex,secondColIndex)) break;
            resultantRow = R_pointer;
            resultantRow.insert(resultantRow.end(),S_pointer.begin(),S_pointer.end());
            cout<< "res row " ;
            printVectorInt(resultantRow);
            resultantTable->writeRow(resultantRow);
            S_pointer = cursor2.getNext("table");
        }
        R_pointer = cursor1.getNext(("table"));

    }
    resultantTable->blockify();
    tableCatalogue.insertTable(resultantTable);
    return;
}

void performNOTEQIJOIN(Table table1,Table table2, vector<string> &columns){

    Table *resultantTable = new Table(parsedQuery.joinResultRelationName, columns);

    Cursor cursor1 = table1.getCursor();
    Cursor cursor2 = table2.getCursor();

    vector<int> R_pointer = cursor1.getNext("table");
    vector<int> S_pointer;
    vector<int> S_marker;

    int firstColIndex = table1.attributeIndexMap[parsedQuery.joinFirstColumnName];
    int secondColIndex = table2.attributeIndexMap[parsedQuery.joinSecondColumnName];
    cout << parsedQuery.joinFirstColumnName << " " <<
         parsedQuery.joinSecondColumnName << " "<<
         firstColIndex << " " <<
         secondColIndex << " " << endl;

    vector<int> resultantRow;
    resultantRow.reserve(resultantTable->columnCount);

    while(!R_pointer.empty()){
        cursor2 = table2.getCursor();
        S_pointer = cursor2.getNext("table");
        while(!S_pointer.empty()){
            if( R_pointer[firstColIndex] != S_pointer[secondColIndex] ){
                resultantRow = R_pointer;
                resultantRow.insert(resultantRow.end(),S_pointer.begin(),S_pointer.end());
                cout<< "res row " ;
                printVectorInt(resultantRow);
                resultantTable->writeRow(resultantRow);
            }
            S_pointer = cursor2.getNext("table");
        }
        R_pointer = cursor1.getNext("table");
    }
    resultantTable->blockify();
    tableCatalogue.insertTable(resultantTable);
    return;
}


void executeJOIN()
{
    logger.log("executeJOIN");

    Table table1 = *(tableCatalogue.getTable(parsedQuery.joinFirstRelationName));
    Table table2 = *(tableCatalogue.getTable(parsedQuery.joinSecondRelationName));
    vector<string> columns;

    //Creating list of column names of the resultant table
    for (int columnCounter = 0; columnCounter < table1.columnCount; columnCounter++){
        string columnName = table1.columns[columnCounter];
        if (table2.isColumn(columnName)){
            columnName = parsedQuery.joinFirstRelationName + "_" + columnName;
        }
        columns.emplace_back(columnName);
    }

    for (int columnCounter = 0; columnCounter < table2.columnCount; columnCounter++){
        string columnName = table2.columns[columnCounter];
        if (table1.isColumn(columnName)){
            columnName = parsedQuery.joinFirstRelationName + "_" + columnName;
        }
        columns.emplace_back(columnName);
    }

    if ( parsedQuery.joinBinaryOperator == EQUAL ){
        performEQUIJOIN(table1,table2,columns);
    }else if( parsedQuery.joinBinaryOperator == LESS_THAN || parsedQuery.joinBinaryOperator == LEQ){
        performLESSTHANCONDITIONALJOIN(table1,table2,columns);
    }else if(parsedQuery.joinBinaryOperator == GREATER_THAN || parsedQuery.joinBinaryOperator == GEQ){
        performGREATERTHANCONDITIONALJOIN(table1,table2,columns);
    }else if( parsedQuery.joinBinaryOperator == NOT_EQUAL){
        performNOTEQIJOIN(table1,table2,columns);
    }
    return;
}