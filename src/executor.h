#include"semanticParser.h"

void executeCommand();

void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT();
void executeINDEX();
void executeJOIN();
void executeLIST();
void executeLOAD();
void executePRINT();
void executePROJECTION();
void executeRENAME();
void executeSELECTION();
void executeSORT();
void executeSOURCE();

// for matrix
void executeCLEARMATRIX();
void executeLOADMATRIX();
void executePRINTMATRIX();
void executeRENAMEMATRIX();
void executeEXPORTMATRIX();

void executeTRANSPOSEMATRIX();
void executeCHECKSYMMETRY();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);