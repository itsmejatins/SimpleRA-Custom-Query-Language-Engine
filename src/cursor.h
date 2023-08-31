#include"bufferManager.h"
/**
 * @brief The cursor is an important component of the system. To read from a
 * table, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
 */
class Cursor{
    public:
    Page page;
    int pageIndex;
    string relationName;
    int pagePointer;

    public:
    Cursor(string relationName, int pageIndex);
    vector<int> getNext(string relationType);
    void nextPage(int pageIndex);
};