#include"global.h"
#include <vector>
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
SORT <table_name> BY <column_name_1, column_name_2,..., column_name_k> IN <ASC|DESC, ASC|DESC,..., ASC|DESC>
 * sorting_order = ASC | DESC 
 */
void printVector(vector<string> stringVector){
    cout << "[" ;
    for( auto str : stringVector){
        cout<< str +" , " ;
    }
    cout << " ] ";
}
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    printVector(tokenizedQuery);
    if( tokenizedQuery[2] != "BY" || tokenizedQuery[6] != "IN"){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = SORT;
//    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    parsedQuery.sortRelationName = tokenizedQuery[1];
//    parsedQuery.sortColumnName = tokenizedQuery[5];
//    string sortingStrategy = tokenizedQuery[7];

    int noAttributes = (tokenizedQuery.size()-4)/2;
    for(int i=0;i<noAttributes;i++) {
        parsedQuery.sortColumnNames.push_back(tokenizedQuery[i + 3]);
        parsedQuery.sortingStrategies.push_back(tokenizedQuery[i + 7]);
    }
//    if(sortingStrategy == "ASC")
//        parsedQuery.sortingStrategy = ASC;
//    else if(sortingStrategy == "DESC")
//        parsedQuery.sortingStrategy = DESC;
//    else{
//        cout<<"SYNTAX ERROR"<<endl;
//        return false;
//    }
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

//    if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
//        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
//        return false;
//    }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }
    for( auto colName : parsedQuery.sortColumnNames ){
        if(!tableCatalogue.isColumnFromTable(colName, parsedQuery.sortRelationName)){
            cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
            return false;
        }
    }

    return true;
}

void executeSORT(){
    logger.log("executeSORT");
    Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    table->sort();
}