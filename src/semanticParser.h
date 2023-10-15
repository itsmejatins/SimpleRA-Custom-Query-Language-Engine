#include"syntacticParser.h"

bool semanticParse();

bool semanticParseCLEAR();
bool semanticParseCROSS();
bool semanticParseDISTINCT();
bool semanticParseEXPORT();
bool semanticParseINDEX();
bool semanticParseJOIN();
bool semanticParseGROUPBY();
bool semanticParseLIST();
bool semanticParseLOAD();
bool semanticParsePRINT();
bool semanticParsePROJECTION();
bool semanticParseRENAME();
bool semanticParseSELECTION();
bool semanticParseSORT();
bool semanticParseSOURCE();
bool semanticParseORDERBY();


// for matrix
bool semanticParseCLEARMATRIX();
bool semanticParseLOADMATRIX();
bool semanticParsePRINTMATRIX();
bool semanticParseEXPORTMATRIX();
bool semanticParseRENAMEMATRIX();

bool semanticParseTRANSPOSEMATRIX();
bool semanticParseCOMPUTEMATRIX();
bool semanticParseCHECKSYMMETRY();