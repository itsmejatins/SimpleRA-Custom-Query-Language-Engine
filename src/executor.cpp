#include"global.h"

void executeCommand(){

    switch(parsedQuery.queryType){
        case CLEAR: executeCLEAR(); break;
        case CROSS: executeCROSS(); break;
        case DISTINCT: executeDISTINCT(); break;
        case EXPORT: executeEXPORT(); break;
        case INDEX: executeINDEX(); break;
        case JOIN: executeJOIN(); break;
        case LIST: executeLIST(); break;
        case LOAD: executeLOAD(); break;
        case PRINT: executePRINT(); break;
        case PROJECTION: executePROJECTION(); break;
        case RENAME: executeRENAME(); break;
        case SELECTION: executeSELECTION(); break;
        case SORT: executeSORT(); break;
        case SOURCE: executeSOURCE(); break;

            // for matrix
        case CLEARMATRIX: executeCLEARMATRIX(); break;
        case LOADMATRIX: executeLOADMATRIX(); break;
        case PRINTMATRIX: executePRINTMATRIX(); break;
        case RENAMEMATRIX: executeRENAMEMATRIX(); break;
        case EXPORTMATRIX: executeEXPORTMATRIX(); break;

        case CHECKSYMMETRY : executeCHECKSYMMETRY(); break;
        case TRANSPOSEMATRIX : executeTRANSPOSEMATRIX(); break;
         case COMPUTEMATRIX : executeCOMPUTEMATRIX(); break;
        default: cout<<"PARSING ERROR"<<endl;
    }
}

void printRowCount(int rowCount){
    cout<<"\n\nRow Count: "<<rowCount<<endl;
}