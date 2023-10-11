#include "global.h"
#include <bits/stdc++.h>
#include <vector>
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
 * tokenizedQuery[8] = aggregate(attribute)                 ->      groupByConditionAggregateFunction
 * tokenizedQuery[9] = bin_op                               ->      BinaryOperator groupByBinaryOperator
 * tokenizedQuery[10] = attribute_value                     ->      unsigned int groupByAggregateThreshold
 * tokenizedQuery[11] = RETURN
 * tokenizedQuery[12] = aggregate_function(attribute)       ->      AggregateFunction groupByReturnAggregateFunction
 */
void printVectorString(vector<string> stringVec){
    cout << " [ " ;
    for( auto str : stringVec ){
        cout << str << " , ";
    }
    cout << " ]" << endl;
}
void printVectorInt(vector<int> intVec){
    cout << " [ " ;
    for( auto str : intVec ){
        cout << str << " , ";
    }
    cout << " ]" << endl;
}
bool syntacticParseGROUPBY()
{
    logger.log("syntacticParseGROUPBY");
    cout << "TOKENIZED QUERY " << endl;
    printVectorString(tokenizedQuery);

    if (tokenizedQuery.size() != 13 || tokenizedQuery[2] != "GROUP" ||
        tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" ||
        tokenizedQuery[7] != "HAVING" || tokenizedQuery[11] != "RETURN" )
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    //set parsedQuery attributes
    parsedQuery.queryType = GROUP;
    parsedQuery.groupByResultRelationName = tokenizedQuery[0];
    parsedQuery.groupingAttribute = tokenizedQuery[4];
    parsedQuery.groupByRelationName = tokenizedQuery[6];
    
    //extracting aggregate(attribute)
    //aggregate func length will always be 3
    // str.substr(0,3) << "\n";
    // str.substr(3+1,str.size()-5);

    string aggregateCondFunc = tokenizedQuery[8].substr(0,3);
    if ( aggregateCondFunc == "MIN" ){
        parsedQuery.groupByConditionAggregateFunction = MIN;
    }else if ( aggregateCondFunc == "MAX" ){
        parsedQuery.groupByConditionAggregateFunction = MAX;
    }else if ( aggregateCondFunc == "SUM" ){
        parsedQuery.groupByConditionAggregateFunction = SUM;
    }else if ( aggregateCondFunc == "COUNT" ){
        parsedQuery.groupByConditionAggregateFunction = COUNT;
    }else if ( aggregateCondFunc == "AVG" ){
        parsedQuery.groupByConditionAggregateFunction = AVG;
    }else{
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.groupByConditionAttribute = tokenizedQuery[8].substr(4,tokenizedQuery[8].size()-5);

    //set group by binary operator
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
    parsedQuery.groupByAggregateThreshold = stoi(tokenizedQuery[10]);
    
    string aggregateReturnFunc = tokenizedQuery[12].substr(0,3);
    if ( aggregateCondFunc == "MIN" ){
        parsedQuery.groupByReturnAggregateFunction = MIN;
    }else if ( aggregateCondFunc == "MAX" ){
        parsedQuery.groupByReturnAggregateFunction = MAX;
    }else if ( aggregateCondFunc == "SUM" ){
        parsedQuery.groupByReturnAggregateFunction = SUM;
    }else if ( aggregateCondFunc == "COUNT" ){
        parsedQuery.groupByReturnAggregateFunction = COUNT;
    }else if ( aggregateCondFunc == "AVG" ){
        parsedQuery.groupByReturnAggregateFunction = AVG;
    }else{
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.groupByReturnAttribute = tokenizedQuery[12].substr(4,tokenizedQuery[12].size()-5);
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

     if (!tableCatalogue.isTable(parsedQuery.groupByRelationName) )
     {
         cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
         return false;
     }

     if (!tableCatalogue.isColumnFromTable(parsedQuery.groupingAttribute, parsedQuery.groupByRelationName) ||
         !tableCatalogue.isColumnFromTable(parsedQuery.groupByConditionAttribute, parsedQuery.groupByRelationName) ||
         !tableCatalogue.isColumnFromTable(parsedQuery.groupByReturnAttribute, parsedQuery.groupByRelationName) )
     {
         cout << parsedQuery.groupingAttribute << endl ;
         cout << parsedQuery.groupByConditionAttribute << endl ;
         cout << parsedQuery.groupByReturnAttribute << endl ;
         cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
         return false;
     }
    return true;
     cout << " seantic group by done " << endl;
}


void executeGROUPBY()
{
    logger.log("executeGROUPBY");
    cout << "LETS EXECUTE GROUP BY " << endl;

    Table table1 = *(tableCatalogue.getTable(parsedQuery.groupByRelationName));
    vector<string> columns;
    columns.emplace_back(parsedQuery.groupingAttribute);
    // fix this
    // columns.emplace_back(parsedQuery.groupByReturnAggregateFunction + parsedQuery.groupByReturnAttribute);

    Table *resultantTable = new Table(parsedQuery.groupByResultRelationName, columns);
    unordered_map<int,vector<int>> hashMap;

    Cursor cursor1 = table1.getCursor();
    vector<int> R_pointer = cursor1.getNext("table");

    int groupingAttributeIndex = table1.attributeIndexMap[ parsedQuery.groupingAttribute];
    int resultAttributeIndex = table1.attributeIndexMap[ parsedQuery.groupByReturnAttribute];

    while( !R_pointer.empty() ){
        if ( hashMap.find(R_pointer[groupingAttributeIndex]) != hashMap.end() ){
            //exits
            hashMap[R_pointer[groupingAttributeIndex]].push_back(R_pointer[resultAttributeIndex]);
        }else{
            hashMap.insert({ R_pointer[groupingAttributeIndex] , {R_pointer[resultAttributeIndex]}});
        }
        R_pointer = cursor1.getNext("table");
    }
    for (const auto& pair : hashMap) {
        int key = pair.first;
        const vector<int>& values = pair.second;

        std::cout << "Key: " << key << ", Values: [";
        for (int value : values) {
            std::cout << value << ", ";
        }
        cout << " ] " << endl;
        
    }
    

    return;
}