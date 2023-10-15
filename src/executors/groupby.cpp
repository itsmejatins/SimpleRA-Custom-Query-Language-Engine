#include "global.h"
#include <iostream>
#include <climits>
#include <vector>
#include <string>

using namespace std;

void sortByGroupingAttribute();
void removeGROUPBYTemporaryTables();
/**
 * @brief
 * SYNTAX: R <- GROUP BY grouping_attribute FROM relation_name HAVING aggregate(attribute) bin_op attribute_value RETURN aggregate_function(attribute)
 * tokenizedQuery[0] = R                                    ->      groupByResultRelationName
 * tokenizedQuery[1] = <-
 * tokenizedQuery[2] = GROUP
 * tokenizedQuery[3] = BY
 * tokenizedQuery[4] = grouping_attribute                   ->      groupingAttribute
 * tokenizedQuery[5] = FROM
 * tokenizedQuery[6] = relation_name                        ->      string groupByRelationName
 * tokenizedQuery[7] = HAVING
 * tokenizedQuery[8] = aggregate(attribute)                 ->      AggregateFunction groupByConditionAggregateFunction
 * tokenizedQuery[9] = bin_op                               ->      BinaryOperator groupByBinaryOperator
 * tokenizedQuery[10] = attribute_value                     ->      unsigned int groupByAggregateThreshold
 * tokenizedQuery[11] = RETURN
 * tokenizedQuery[12] = aggregate_function(attribute)       ->      AggregateFunction groupByReturnAggregateFunction
 */
void printVectorString(vector<string> stringVec)
{
    cout << " [ ";
    for (auto str : stringVec)
    {
        cout << str << " , ";
    }
    cout << " ]" << endl;
}
void printVectorInt(vector<int> intVec)
{
    cout << " [ ";
    for (auto str : intVec)
    {
        cout << str << " , ";
    }
    cout << " ]" << endl;
}

bool isNumber(const std::string &s)
{
    try
    {
        size_t pos;
        std::stod(s, &pos);     // Attempt to convert the string to a double
        return pos == s.size(); // Check if the entire string was used in the conversion
    }
    catch (...)
    {
        // std::invalid_argument or std::out_of_range exception indicates conversion failure
        return false;
    }
}

bool syntacticParseGROUPBY()
{
    logger.log("syntacticParseGROUPBY");
    sortByGroupingAttribute();
    cout << "TOKENIZED QUERY " << endl;
    printVectorString(tokenizedQuery);
    vector<string> ctq; // condensedTokenizedQuery

    if (isNumber(tokenizedQuery.at(tokenizedQuery.size() - 1)))
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    for (int i = 0; i < tokenizedQuery.size();)
    {
        string s = tokenizedQuery.at(i);
        if (isNumber(s))
        {
            i++;
            while (isNumber(tokenizedQuery.at(i)))
            {
                s += tokenizedQuery.at(i);
                i++;
            }
            ctq.push_back(s);
            continue;
        }
        else
            ctq.push_back(s);
        i++;
    }
    if (ctq[2] != "GROUP" ||
        ctq[3] != "BY" || ctq[5] != "FROM" ||
        ctq[7] != "HAVING" || ctq[ctq.size() - 2] != "RETURN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    // set parsedQuery attributes
    parsedQuery.queryType = GROUP;
    parsedQuery.groupByResultRelationName = tokenizedQuery[0];
    parsedQuery.groupingAttribute = tokenizedQuery[4];
    parsedQuery.groupByRelationName = tokenizedQuery[6]+"_sorted";

    // extracting aggregate(attribute)
    //  str.substr(startingIndex, length)
    string aggregateCondFunc = tokenizedQuery[8].substr(0, 3);
    parsedQuery.groupByConditionAttribute = tokenizedQuery[8].substr(4, tokenizedQuery[8].size() - 5);

    cout << "aggregateCondFunc " << aggregateCondFunc << endl;
    if (aggregateCondFunc == "MIN")
    {
        parsedQuery.groupByConditionAggregateFunction = MIN;
    }
    else if (aggregateCondFunc == "MAX")
    {
        parsedQuery.groupByConditionAggregateFunction = MAX;
    }
    else if (aggregateCondFunc == "SUM")
    {
        parsedQuery.groupByConditionAggregateFunction = SUM;
    }
    else if (aggregateCondFunc == "COU")
    {
        string aggregateCondFuncFULL = tokenizedQuery[8].substr(0, 5);
        cout << "aggregateCondFuncFULL " << aggregateCondFuncFULL << endl;
        if(aggregateCondFuncFULL == "COUNT"){
            parsedQuery.groupByConditionAggregateFunction = COUNT;
            parsedQuery.groupByConditionAttribute = tokenizedQuery[8].substr(6, tokenizedQuery[8].size() - 7);
        }
        else{
            cout << "SYNTAX ERROR "<< endl;
            return false;
        }
    }
    else if (aggregateCondFunc == "AVG")
    {
        parsedQuery.groupByConditionAggregateFunction = AVG;
    }
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    

    // set group by binary operator
    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "<")
        parsedQuery.groupByBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.groupByBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.groupByBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.groupByBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.groupByBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.groupByBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.groupByAggregateThreshold = stoi(ctq[10]);

    string aggregateReturnFunc = ctq[ctq.size() - 1].substr(0, 3);
    parsedQuery.groupByReturnAttribute = ctq[ctq.size() - 1].substr(4, ctq[ctq.size() - 1].size() - 5);
    parsedQuery.groupByResultRelationAttribute = aggregateReturnFunc + parsedQuery.groupByReturnAttribute;

    if (aggregateReturnFunc == "MIN")
    {
        parsedQuery.groupByReturnAggregateFunction = MIN;
    }
    else if (aggregateReturnFunc == "MAX")
    {
        parsedQuery.groupByReturnAggregateFunction = MAX;
    }
    else if (aggregateReturnFunc == "SUM")
    {
        parsedQuery.groupByReturnAggregateFunction = SUM;
    }
    else if (aggregateReturnFunc == "COU")
    {
        string aggregateReturnFuncFULL = ctq[ctq.size() - 1].substr(0, 5);
        if(aggregateReturnFuncFULL == "COUNT"){
            parsedQuery.groupByReturnAggregateFunction = COUNT;
            parsedQuery.groupByReturnAttribute = ctq[ctq.size() - 1].substr(6, ctq[ctq.size() - 1].size() - 7);
            parsedQuery.groupByResultRelationAttribute = aggregateReturnFuncFULL + parsedQuery.groupByReturnAttribute;
        }
        else{
            cout << "SYNTAX ERROR "<< endl;
            return false;
        }
    }
    else if (aggregateReturnFunc == "AVG")
    {
        parsedQuery.groupByReturnAggregateFunction = AVG;
    }
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    

    
    cout << "parameters initialized " << endl;
    return true;
}

bool semanticParseGROUPBY()
{
    logger.log("semanticParseGROUPBY");
    if (tableCatalogue.isTable(parsedQuery.groupByResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.groupByRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupingAttribute, parsedQuery.groupByRelationName) ||
        !tableCatalogue.isColumnFromTable(parsedQuery.groupByConditionAttribute, parsedQuery.groupByRelationName) ||
        !tableCatalogue.isColumnFromTable(parsedQuery.groupByReturnAttribute, parsedQuery.groupByRelationName))
    {
        cout << parsedQuery.groupingAttribute << endl;
        cout << parsedQuery.groupByConditionAttribute << endl;
        cout << parsedQuery.groupByReturnAttribute << endl;
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
    cout << " seantic group by done " << endl;
}

double performConditionAggregation(int conditionValue, int currVal, int count)
{
    if (parsedQuery.groupByConditionAggregateFunction == MIN)
    {
        return min(conditionValue, currVal);
    }
    else if (parsedQuery.groupByConditionAggregateFunction == MAX)
    {
        return max(conditionValue, currVal);
    }
    else if (parsedQuery.groupByConditionAggregateFunction == SUM)
    {
        return conditionValue + currVal;
    }
    else if (parsedQuery.groupByConditionAggregateFunction == COUNT)
    {
        return count;
    }
    else if (parsedQuery.groupByConditionAggregateFunction == AVG)
    {
        return ((double)(conditionValue * (count - 1) + currVal)) / (double)count;
    }
    else
    {
        return INT_MIN;
    }
}
int performReturnAggregation(int returnValue, int currVal, int count)
{
    if (parsedQuery.groupByReturnAggregateFunction == MIN)
    {
        if( returnValue == 0 ){ return currVal; }
        return min(returnValue, currVal);
    }
    else if (parsedQuery.groupByReturnAggregateFunction == MAX)
    {
        return max(returnValue, currVal);
    }
    else if (parsedQuery.groupByReturnAggregateFunction == SUM)
    {
        return returnValue + currVal;
    }
    else if (parsedQuery.groupByReturnAggregateFunction == COUNT)
    {
        return count;
    }
    else if (parsedQuery.groupByReturnAggregateFunction == AVG)
    {
        return ((double)(returnValue * (count - 1) + currVal)) / (double)count;
    }
    else
    {
        return INT_MIN;
    }
}

void sortByGroupingAttribute(){
    //first sort both the relations by grouping attribute

    //first relation
    parsedQuery.orderByRelation = tokenizedQuery[6];
    parsedQuery.orderByResultRelation = tokenizedQuery[6]+"_sorted";
    parsedQuery.orderByCol = tokenizedQuery[4];
    parsedQuery.orderByStrategy = "ASC";
    parsedQuery.queryType = ORDER_BY;

    //execute order by
    executeORDERBY();

    parsedQuery.clear();
}

void executeGROUPBY()
{

    logger.log("executeGROUPBY");

    cout << "LETS EXECUTE GROUP BY " << parsedQuery.groupByRelationName << endl;

    Table table1 = *(tableCatalogue.getTable(parsedQuery.groupByRelationName));
    vector<string> columns;

    columns.emplace_back(parsedQuery.groupingAttribute);
    columns.emplace_back(parsedQuery.groupByResultRelationAttribute);

    printVectorString(columns);

    Table *resultantTable = new Table(parsedQuery.groupByResultRelationName, columns);
    vector<int> resultantRow;
    resultantRow.reserve(resultantTable->columnCount);

    int groupingAttributeIndex = table1.attributeIndexMap[parsedQuery.groupingAttribute];
    int conditionAttributeIndex = table1.attributeIndexMap[parsedQuery.groupByConditionAttribute];
    int resultAttributeIndex = table1.attributeIndexMap[parsedQuery.groupByReturnAttribute];

    Cursor cursor1 = table1.getCursor();
    vector<int> R_pointer = cursor1.getNext("table");
    int groupingAttributeVal = R_pointer[groupingAttributeIndex];

    double conditionValue = 0;
    double returnValue = 0;
    int count = 1;

    while (!R_pointer.empty())
    {
        if (R_pointer[groupingAttributeIndex] != groupingAttributeVal)
        {

            // check threshold  condition
            if (evaluateBinOp(conditionValue, parsedQuery.groupByAggregateThreshold, parsedQuery.groupByBinaryOperator)){
                // form the result row
                // first goes the grouping attribute
                resultantRow.emplace_back(groupingAttributeVal);
                // inserting return value
                resultantRow.emplace_back(returnValue);

                // write the row
                cout << "res row ";
                printVectorInt(resultantRow);
                resultantTable->writeRow<int>(resultantRow);
                resultantRow.clear();
            }
            // reset the values
            count = 1;
            conditionValue = 0;
            returnValue = 0;
            groupingAttributeVal = R_pointer[groupingAttributeIndex];

            // consider the current value and finding the condition value and return value
            conditionValue = performConditionAggregation(conditionValue, R_pointer[conditionAttributeIndex], count);
            returnValue = performReturnAggregation(returnValue, R_pointer[resultAttributeIndex], count);
            count++;
        }
        else
        {
            conditionValue = performConditionAggregation(conditionValue, R_pointer[conditionAttributeIndex], count);
            returnValue = performReturnAggregation(returnValue, R_pointer[resultAttributeIndex], count);
            count++;
        }
        R_pointer = cursor1.getNext("table");
    }
    // create row for the last grouping attribute value
    if (evaluateBinOp(conditionValue, parsedQuery.groupByAggregateThreshold, parsedQuery.groupByBinaryOperator)){
        // form the result row
        // inserting grouping attribute value
        resultantRow.emplace_back(groupingAttributeVal);
        // inserting the return attribute value
        resultantRow.emplace_back(returnValue);
        
        // write the row
        cout << "res row ";
        printVectorInt(resultantRow);
        resultantTable->writeRow<int>(resultantRow);
        resultantRow.clear();
    }

    if ( resultantTable->rowCount == 0 ){
        //result is empty
        resultantRow.emplace_back(INT_MIN);
        resultantRow.emplace_back(INT_MIN);
        resultantTable->writeRow<int>(resultantRow);
    }
    resultantTable->blockify();
    tableCatalogue.insertTable(resultantTable);
    removeGROUPBYTemporaryTables();

    string deleteFileName = resultantTable->sourceFileName;
    remove(&deleteFileName[0]);

    return;
}

void removeGROUPBYTemporaryTables(){
    parsedQuery.queryType = CLEAR;
    parsedQuery.clearRelationName = tokenizedQuery[6]+"_sorted";
    executeCLEAR();
    parsedQuery.clear();
}