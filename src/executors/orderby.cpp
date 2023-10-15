#include "global.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

bool syntacticParseORDERBY()
{
    if(tokenizedQuery.size() != 8)
        return false;
    if(tokenizedQuery[6] != "ON")
        return false;
    if(tokenizedQuery[5] != "ASC" && tokenizedQuery[5] != "DESC")
        return false;

    parsedQuery.orderByRelation = tokenizedQuery[7];
    parsedQuery.orderByResultRelation = tokenizedQuery[0];
    parsedQuery.orderByCol = tokenizedQuery[4];
    parsedQuery.orderByStrategy = tokenizedQuery[5];
    parsedQuery.queryType = ORDER_BY;
    return true;
}

bool semanticParseORDERBY()
{
    if(!tableCatalogue.isTable(parsedQuery.orderByRelation))
    {
        cerr << "Relation does not exists" << endl;
        return false;
    }
    if(tableCatalogue.isTable(parsedQuery.orderByResultRelation))
    {
        cerr << "The resulting relation name already exists, please choose a different name" << endl;
        return false;
    }
    if(!tableCatalogue.isColumnFromTable(parsedQuery.orderByCol, parsedQuery.orderByRelation))
    {
        cerr << "The column is not present in the relation" << endl;
        return false;
    }

    return true;
}

void executeORDERBY()
{
    Table *table = tableCatalogue.getTable(parsedQuery.orderByRelation);
    table->orderBy();
    return;
}