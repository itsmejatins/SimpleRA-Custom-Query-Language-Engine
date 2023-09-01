#include "global.h"
#include <string>
BufferManager::BufferManager()
{
    logger.log("BufferManager::BufferManager");
}

/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param relationName
 * @param pageIndex
 * @return Page
 */
Page BufferManager::getPage(string relationName, int pageIndex)
{
    logger.log("BufferManager::getPage");
    string pageName = "../data/temp/"+relationName + "_Page" + to_string(pageIndex);
    if (this->inPool(pageName))
        return this->getFromPool(pageName);
    else
        return this->insertIntoPool(relationName, pageIndex);
}

/**
 * @brief Checks to see if a page exists in the pool
 *
 * @param pageName
 * @return true
 * @return false
 */
bool BufferManager::inPool(string pageName)
{
    logger.log("BufferManager::inPool");
    for (auto page : this->pages)
    {
        if (pageName == page.pageName)
            return true;
    }
    return false;
}

/**
 * @brief If the page is present in the pool, then this function returns the
 * page. Note that this function will fail if the page is not present in the
 * pool.
 *
 * @param pageName
 * @return Page
 */
Page BufferManager::getFromPool(string pageName)
{
    logger.log("BufferManager::getFromPool");
    for (auto page : this->pages)
        if (pageName == page.pageName)
            return page;
}

/**
 * @brief Inserts page indicated by relationName and pageIndex into pool. If the
 * pool is full, the pool ejects the oldest inserted page from the pool and adds
 * the current page at the end. It naturally follows a queue data structure.
 *
 * @param relationName
 * @param pageIndex
 * @return Page
 */
Page BufferManager::insertIntoPool(string relationName, int pageIndex)
{
    logger.log("BufferManager::insertIntoPool");
    Page page(relationName, pageIndex);
    if (this->pages.size() >= BLOCK_COUNT)
        pages.pop_front();
    pages.push_back(page);
    return page;
}

/**
 * @brief The buffer manager is also responsible for writing pages. This is
 * called when new tables are created using assignment statements.
 *
 * @param relationName
 * @param pageIndex
 * @param rows
 * @param rowCount
 */
void BufferManager::writePage(string relationName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("BufferManager::writePage");
    Page page(relationName, pageIndex, rows, rowCount);
    page.writePage();

}

/**
 * @brief Deletes file names fileName
 *
 * @param fileName
 */
void BufferManager::deleteFile(string fileName)
{

    if (remove(fileName.c_str()))
        logger.log("BufferManager::deleteFile: Err");
    else logger.log("BufferManager::deleteFile: Success");
}

/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the relationName and pageIndex.
 *
 * @param relationName
 * @param pageIndex
 */
void BufferManager::deleteFile(string relationName, int pageIndex)
{
    logger.log("BufferManager::deleteFile");
    string fileName = "../data/temp/" + relationName + "_Page" + to_string(pageIndex);
    this->deleteFile(fileName);
}