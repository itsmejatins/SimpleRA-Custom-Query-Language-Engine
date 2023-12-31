#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");
    switch(parsedQuery.queryType){
        case CLEAR: return semanticParseCLEAR();
        case CROSS: return semanticParseCROSS();
        case DISTINCT: return semanticParseDISTINCT();
        case EXPORT: return semanticParseEXPORT();
        case INDEX: return semanticParseINDEX();
        case JOIN: return semanticParseJOIN();
        case GROUP: return semanticParseGROUPBY();
        case LIST: return semanticParseLIST();
        case LOAD: return semanticParseLOAD();
        case PRINT: return semanticParsePRINT();
        case PROJECTION: return semanticParsePROJECTION();
        case RENAME: return semanticParseRENAME();
        case SELECTION: return semanticParseSELECTION();
        case SORT: return semanticParseSORT();
        case SOURCE: return semanticParseSOURCE();
        case ORDER_BY: return semanticParseORDERBY();

            // for matrix
        case CLEARMATRIX: return semanticParseCLEARMATRIX();
        case LOADMATRIX: return semanticParseLOADMATRIX();
        case PRINTMATRIX: return semanticParsePRINTMATRIX();
        case RENAMEMATRIX: return semanticParseRENAMEMATRIX();
        case EXPORTMATRIX: return semanticParseEXPORTMATRIX();

        case TRANSPOSEMATRIX: return semanticParseTRANSPOSEMATRIX();
        case COMPUTEMATRIX : return semanticParseCOMPUTEMATRIX();
        case CHECKSYMMETRY : return semanticParseCHECKSYMMETRY();

        default: cout<<"SEMANTIC ERROR"<<endl;
    }

    return false;
}