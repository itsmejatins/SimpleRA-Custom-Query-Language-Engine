# **DATA SYSTEMS**

### **Project Phase - 1**
### **TASK : Handle Matrices** <br/>

### **1. Introduction**
We have created new classes for MATRIX relation type. We wanted to decouple the matrix functionalities from the table functionalities so as to scale-up/scale-down on the features and efficiency of features for matrix.

Created Matrix class and MatrixCatalogue class.
We have also made some variable name (table --> relation) changes in other classes which are common for both.

### **2. Command Description** <br/>

#### **2.a LOAD MATRIX** <br/>
We have created new functions in load.cpp . These functions check the syntax, semantic and loads the matrix into pages.

Syntax checks - The commands should have 3 words. LOAD, MATRIX & the matrix name. Otherwise it will throw a syntax error.

Semantic checks - We are checking if the matrix is already loaded or there is any other matrix with the same name. Otheriwse it throws a semantic error.

We have used very similar LOAD functionality as compared to the table. We are dividing the matrix into rows and storing them in pages. Total rows in a page will be floor(pageSize/rowSize) and total pages will be (total rows*row size)/page size.

Total page accesses will be equal to the total pages required as each page is getting accessed once and the entire row(s) are being written at once.

<br/>

#### **2.b PRINT MATRIX** <br/>

We have created new functions in print.cpp . These functions check the syntax, semantic and prints the matrix by reading all the pages.

Syntax checks - The commands should have 3 words. PRINT, MATRIX & the matrix name. Otherwise it will throw a syntax error.

Semantic checks - We are checking if the matrix is already loaded. If not, it throws a semantic error.

This reads the pages based on the page number using cursors and prints the matrix row-by-row.

<br/>

#### **2.c TRANSPOSE MATRIX**<br/>
We have a new class matrixTranspose.cpp which has different functions for syntax and semantic check and perform the transpose of the matrix.

Syntax checks - The commands should have 3 words. TRANSPOSE, MATRIX & matrix name. Otherwise it will throw a syntax error.

Semantic checks - This function checks if the matrix is already loaded. If not, it throws a semantic error.

Transpose Matix - This function calculates the transpose of an loaded matrix. We have used the conventional method of taking the transpose, where we swap the complete row with the complete column.<br/><br/>

We created few helper functions to ease the functionality of transpose<br/>

- writeRowToTemporaryBlock - This function reads the block and fetches one particular row and write that to a temporary block<br/>

- writeColToTemporaryBlock - This function reads all the blocks and fetches one particular column and write that to a temporary block.

- updateRow - This function read the temporary block(column) and update the values of the corresponding row.

- updateCol - This function read the temporary block(row) and update the values of the corresponding row.

There were few other helper functions to read a page (**readPage**), and to write or read a vector from a page (**writeVectorToFile**, **writeRowToTemporaryBlock** , **writeColToTemporaryBlock** )

**Algorithm** <br/>
for row in rows of matrix :<br/>
- fetch i-th row and write that to a temporary block(row) - **writeRowToTemporaryBlock**<br/>
- fetch ith column and write that to a temporary block(column) - **writeColToTemporaryBlock**<br/
- read the temporary block(row) and update the column - **updateRow**.
- read the temporary block(column) and update the row - **updateCol**.

<br/>

#### **2d. EXPORT MATRIX** <br/>
We have created new functions in export.cpp . These functions check the syntax, semantic and prints the matrix by reading all the pages.

Syntax checks - The commands should have 4 words. EXPORT, MATRIX & matrix name. Otherwise it will throw a syntax error.

Semantic checks - We are checking if the matrix is already loaded. If not, it throws a semantic error.

We will use the same logic which is used to export tables. By reading each page page number wise and writing the new line row by row. But as this is a matrix, we are not writing columns. Again, the total page accesses here will be equal to total pages as we are accessing each page just once.

<br/>

#### **2e. RENAME MATRIX** <br/>
- We have created new functions in rename.cpp . These functions check the syntax, semantic and prints the matrix by reading all the pages.

- Syntax checks - The commands should have 4 words. RENAME, MATRIX, from matrix name & to matrix name. Otherwise it will throw a syntax error.

- Semantic checks - We are checking if the matrix is already loaded. If not, it throws a semantic error. We also check for the other matrix name, if that is already loaded. Again it will throw an error if that is loaded too.

- We are creating new pages with the new name matrix and loading the matrix from the previous source. To be more memory efficient, we are removing the pages of previous name matrix. And the source of matrix doesn't change as we do not want multiple csv files on every RENAME.

- A function is added in syntacticParse.cpp which check that at any given location a file exists or not. This was done because after renaming the funciton, the name of the matrix in memory is different from the name of its original csv file. To check whether the matrix is loaded in the main memory before not, we first check if its original file is present or not. If the origial file is absent, we check if the first page of the renamed matrix is present in temp folder or not. If the page is present, the semantic check is passed, else false is returned.

<br/>

#### **2f. CHECKSYMMETRY** <br/>
For checking symmetry we have a new class matrixCheckSymmetry.cpp which has different functions for syntax and semantic check and to check the symmetry of the matrix.

Syntax checks - The commands should have 2 words. CHECKSYMETRY & matrix name. Otherwise it will throw a syntax error.

Semantic checks - This function throuws a semantic error if the matrix is not loaded.

Chek Symetry Matix - This function checks for the symmetry of the matrix by using the helper function of transpose matrix <br/>

In symmetry we are extracting i-th row and i-th column and then checking whether all the elements at the corresponding position are equal or not.

**Algorithm** to find the transpose of the matrix : <br/><br/>
for row in rows of matrix :<br/>
- fetch i-th row and write that to a temporary block(row) - **writeRowToTemporaryBlock**<**br/>
- fetch ith column and write that to a temporary block(column) - **writeColToTemporaryBlock**<br/>
- read both the temporary blocks (row & column)
- if content of both the temporary block are different then the matrix is not symmetric and return false - **checkTempBlocks**.


<br/>

#### **2g. COMPUTE** <br/>
- For computing A -A', in the executor folder, matrixCompute.cpp file has been created. It has three functions each for checking syntax, semantics and executing the compute function respectively.
- Logic for syntax checking is very similar to other commands. The enum variable responsible for identifying the query as compute query 'COMPUTEMATRIX' has been added in the ParsedQueryClass.
- Inside the ParsedQuery class, the variable computeRelationName stores the name of the matrix upon which compute is being done.

**Algorithm** for computing is as follows -
- The matrix for which A- A' is to be done is loaded in the memory.
- A copy of the original file (if necessary) of this matrix is created as backup. Now all the changes which have been done to this matrix are exported. This updates the original file of the matrix using the temporary pages that were once created while loading the matrix.
- Now we create copy of this updated matrix and the name of the copied file is A_RESULT assuming the original file is A.csv.
- From inside the compute() method, a matrix is loaded. Its name is assigned as A_RESULT. Doing this we have already blockified A_RESULT.csv and its blocks are stored in temp folder. Also we have a matrix called A_RESULT in matrixCatalogue.
- Now we transpose A_RESULT thereby updating the pages of A_RESULT.
- After transposing, we delete the A_RESULT.csv file and A.csv file. The name of the backup file (if created) is changed to A.csv.
- All the pages of A and A_RESULT are accessed one by one. These are stored in memory in two vectors v1 and v2.
- All the corresponding entries of v1 and v2 are subtracted and stored in v2 itself. The updated v2 is then overwritten to the corresponding page of A_RESULT.


<br/>

### **3. Assumptions** <br/>
- We are storing the elements of the matrix in a row wise manner in the block. Therefore to accomodate 1 row according to the given constraing( n=10^4 ) we need to have BLOCK_COUNT as 40.
- We have assumed that the matrix will only contain integers and not floating point decimals or any long values.
- Though our block accesses are more, this can be optmized by using block matrix transposition.

### **4. Learnings** <br/>
- Gained insight as to how to store matrix data in a block efficiently. Also got more expose as to how to store any type of data efficiently with respect to storing and retrieving.
- Finding transpose of the matrix in-place.

### **5. Contribution** <br/>

**Jatin Sharma**
<br>
 - COMPUTE
 - TRANSPOSE
 - LOAD
 - PRINT

**Harshit Kashyap**
<br>
- TRANSPOSE
- EXPORT
- PRINT
- LOAD

**Yash Pathak**
<br>
- CHECKSYMMETRY
- RENAME
- TRANSPOSE
- EXPORT

# PROJECT PHASE 2 REPORT

# TASK 1 - IMPLEMENTATION OF EXTERNAL SORTING

## Syntax

```bash
SORT <table_name> BY <column_name1, column_name2,..., column_namek> IN <ASC|DESC, ASC|DESC,..., ASC|DESC>
```

## Parsing

The methods for parsing were implemented in executors/sort.cpp.

### Syntactic Parse

The class ParsedQuery was modified to include the following attributes -

- `vector<string> sortColumnNames` → It is a vector<string> containing the names of the columns as per which the relation has to be sorted. Note that the relative ordering of the column name matters with the highest priority being given to the initial columns.
- `string sortRelationName` → This is the name of the relation on which sorting is to be performed.
- `vector<string> sortingStrategies` → This is a vector<string> containing the order in which the respective column names must be sorted. It can contain two values - ‘ASC’ or ‘DESC’ representing ascending and descending order respectively.

These attributes are assigned to parsedQuery object from the input through the tokenizedQuery vector.

### Semantic Parse

- In this we check if `tableCatalogue` contains a table with name `sortRelationName`. If yes, then we check for validity of columns otherwise the parser returns false.
- In the next step, using the method `tableCatalogue.isColumnFromTable()`, we check whether the entered columns are actually columns of the existing table or not. If not we return false, otherwise semantic parse is now successful as the table entered as well as its column are all valid. All that remains is now to actually sort the table.

## Sorting implementation

- We created a method in the Table class called sort(), which upon being called sorts the original table. The implementation of the sort() method is done in table.cpp file.
- It is to be noted that the original CSV file remains the same, ie, only the pages of the table gets sorted after sort function is called. This is to ensure that sorting does not modifies the original data. It the sorted table has to be saved, the user always has the option to EXPORT the table.
- A 2-way external merge sort strategy has been implemented which uses 2 memory buffers at a time which is within the given constraints.

### Stage - 1 : Initial run

- We created an additional attribute `unordered_map<string,int> attributeIndexMap` in the Table class. This is a map which stores the indices of the columns of the table. For example if C1 is the 3rd column of the table, then `attributeIndexMap["C1"] = 3`. This map will be used in the comparator of the sort function.
- Using the map above, it initializes the `vector<int> sortColIndices` present in table.cpp file. This vector is then used inside to the comparator `bool compareTwoVectors(const vector<int>& a, const vector<int>& b)` to identify the order in which column needs to be sorted.
- All the pages in the temp folder that belong to the table are sorted by bringing them into the memory one by one and using `sort()` function with the above comparator and then writing them back to the secondary storage.

### Stage -2 : Further runs

- For `ceil(log2(b + 1))` levels, where b is the number of blocks of the table in 0 based indexing, we do the following -
    - maintain two pointers `i` and `j` separated by `jump` number of blocks.
    - pick the appropriate smaller/larger entry (as per the comparator) from the top of the blocks pointed by i and j respectively.
    - Create a new block in the hard drive if not created and write that entry over it.
    - If the written block reaches its max size, then create another block. Keep repeating this until i reaches iFinal and j reaches jFinal.
    - Repeat this for the new set of i and j blocks at current same level. Do this till all the blocks have been traversed for the current level.
    - After each level, the `jump` will become twice of its previous value. The first pointer goes from `i` to `i + jump - 1` block and the second pointer goes from `iFinal + 1` to `min(j + jump - 1, b)` block.
    - Keep repeating the steps until we have all the blocks in the sorted order.
- At every new level, the new pages are appended with $ to distinguish between the current and the previous level. If the previous level already had pages with $ sign appended, we remove the $ sign again and create new level pages without $ sign.
- Appropriate edge cases are handled which can be seen in the code.

![Untitled](PROJECT%20PHASE%202%20REPORT%20ea11ebe6a9744d1b8d9116b3c70082e3/Untitled.png)

### Complexity

- The time complexity is O(n logn)
- The space complexity is O(n).
- Main memory used = 2 block size.

# TASK 1: PART A - JOINS

## Syntax

```
<new_relation_name> <- JOIN <tablename1>, <tablename2> ON <column1> <bin_op> <column2>
```

## Parsing

executors/join.cpp was created. The methods for parsing are in this file

### Syntactic parse

Following  attributes were created in ParsedQuery class -

- queryType → JOIN
- joinResultRelationName → The name of the relation of created after the join operation.
- joinFirstRelationName → the name of the first relation upon which order by has to be done.
- joinSecondRelationName → the name of the second relation upon which order by has to be done.
- joinFirstColumnName → the name of the column of the first relation upon which join operation has to be done.
- joinSecondColumnName → the name of the column of the second relation upon which join operation has to be done.
- joinBinaryOperator → the binary operator

### Semantic parse

- First we will check whether the resultant relation exists or not.
- Then both the relations on which the join has to be performed will be checked whether they exist or not
- Then the attributes on which the joins will be performed will be checked whether they exist in the respective relation or not.

## Implementation details

- Before performing join operation, both the relations will be ordered on the attribute on which the join is to be performed.
- The first relation is considered as R and the second relation is considered as S
- The R_pointer and S_pointer denotes the pointer in the relation R and S respectively
- JOIN operation is broken down into 4 categories :
    - **EQUIJOIN**
        - Both the R_pointer and S_pointer are set to the beginning of the relation
        - If the value of  join attribute of the R_pointer is less than the join attribute of the S_pointer, we increment the R_pointer
        - If the value of  join attribute of the S_pointer is less than the join attribute of the R_pointer, then we increment the S_pointer
        - After both the above conditions have been satisfied, we encounter the rows where the value of joining attribute is equal, we consider the row and write it to the result.
        - The below animation depicts how equi join is performed in our codebase.

          ![ezgif.com-video-to-gif.gif](PROJECT%20PHASE%202%20REPORT%20ea11ebe6a9744d1b8d9116b3c70082e3/ezgif.com-video-to-gif.gif)

    - **NOT EQUI JOIN**
        - This is much similar to cross join.
        - Both the R_pointer and S_pointer are set to the beginning of the relation.
        - Both the relations were iterated , and whenever the value of join attributes did not match, they we included in the result
        - The below animation depicts how not equi join is performed in our codebase.

          ![ezgif.com-video-to-gif.gif](PROJECT%20PHASE%202%20REPORT%20ea11ebe6a9744d1b8d9116b3c70082e3/ezgif.com-video-to-gif%201.gif)

    - **LESS THAN or LESS THAN EQUAL**
        - Both the R_pointer and S_pointer are set to the beginning of the relation.
        - If the value of join attribute of first relation is greater( or equal to ) than the value of join attribute of second relation, then we increment the S_pointer.
        - The row at which the value of join attribute of first relation is less than ( or equal to) we put a S_marker so that from next iteration we will consider from this point.
        - If the value of join attribute of first relation is less ( or equal to ) than the value of join attribute of second relation, then we add that to the result set
        - Once the value of join attribute of both the relation matches the condition, we iterate till the end of the second relation and consider that in the result.
        - The below animation depicts how less than join is performed in our codebase.

          ![ezgif.com-video-to-gif.gif](PROJECT%20PHASE%202%20REPORT%20ea11ebe6a9744d1b8d9116b3c70082e3/ezgif.com-video-to-gif%202.gif)

    - **GREATER THAN or GREATER THAN EQUAL**
        - Both the R_pointer and S_pointer are set to the beginning of the relation.
        - If the value of join attribute of first relation is less( or equal to ) than the value of join attribute of second relation, then we increment the R_pointer.
        - If the value of join attribute of first relation is greater( or equal to ) than the value of join attribute of second relation, then we add it to the result
        - The row at which the condition evaluates to false, we break the loop and increment the R_pointer
        - The below animation depicts how greater than join is performed in our codebase.

          ![ezgif.com-video-to-gif.gif](PROJECT%20PHASE%202%20REPORT%20ea11ebe6a9744d1b8d9116b3c70082e3/ezgif.com-video-to-gif%203.gif)


- In each join operation, in every iteration the join condition is matched, the row is inserted to the resultant table.
- Once the iteration ends, the table will be blockified and inserted.

# TASK 2 : PART B - ORDER BY

## Syntax

```
Result <- ORDER BY Year_Released ASC ON Movies
```

## Parsing

executors/orderby.cpp was created. The methods for parsing are in this file

### Syntactic parse

Following  attributes were created in ParsedQuery class -

- orderByRelation → The name of the relation upon which ORDER BY has to be done.
- orderByResultRelation → The name of the relation of created after the operation.
- orderByCol → the name of the column of the relation upon which order by has to be done.
- orderByStrategy → ‘ASC’ or ‘DSC’.

### Semantic parse

- First the relation on which the order by has to be done has to be present in the tableCatalogue. This is checked.
- Then a relation with the name same as resulting relation should not exist in the table catalogue.

## ORDER BY implementation

- A method `orderBy()` was created in the Table class and corresponding implementation `void Table::orderBy()` was added in table.cpp file.
- It must be noted that the relation upon which ORDER BY has to be performed is in the memory. We may have modified the relation after bringing it to the memory. This means that the contents of the CSV file and the contents of the pages of the relation which are present in the temp folder may be different.
- Therefore we first create a temporary CSV file from the modified pages of the table. The name of this csv file is same as the name of the result of order by table so that we can later blockify it.
- After creating the CSV file, we create a new table from it. This table has the name of orderByResultRelation, and that is why we named the temp CSV file as <orderByResultRelation>.csv. Now the sorting operation for the order by query will be done on these newly created blocks. In this way we maintain consistency among the file present in the secondary storage and the blocks of the table.
- After creating blocks, we call sort() method of this new table with the appropriate sorting strategy received from the tokenizedQuery.
- The resultant table is still not saved. To save this new relation, the user can EXPORT the table.

# TASK 2 : PART C → GROUP BY

### Syntax

```sql
R <- GROUP BY grouping_attribute FROM relation_name HAVING aggregate(attribute) bin_op attribute_value RETURN aggregate_function(attribute)
```

## Parsing

executors/groupby.cpp was created which contains the method for parsing.

### Syntactic parse

- Following attributes have been added to ParseQuery class -
    - `groupByResultRelationName` → name of the resultant relation after group by query.
    - `groupingAttribute` → name of the attribute upon which grouping will be done.
    - `groupByRelationName` → name of the relation upon which group by query will be performed.
    - `groupByConditionAttribute` →
    - `groupByReturnAttribute` → the attribute which needs to be added in the resultant table.
    - `groupByResultRelationAttribute` →
- Basic syntactic check is first done using the length and the words of the query.
- The aggregate functions were identified.
- To include the comma condition in the digits (50,000 === 50000), we did the following -
    - created a function to check whether a string is a number of not → `bool isNumber(const std::string &s)`.
    - Now we created another variable `vector<string> ctq` which represents condensedTokenizedQuery. In this we copy all the string from the `tokenizedQuery` after merging all the comma separated digits into one.
    - For example if tokenizedQuery has multiple strings {”5”, “00”, “000”} that were created after tokenizing the string “5,00,000”, `ctq` will have only {“500000”} along with the rest of the query.
- Now using `ctq`, we can easily find the values for `groupByBinaryOperator`, etc.

### Semantic parse

- The validity of the names of the table and its column is checked.
- We also check whether the resulting relation name is already loaded in the memory or not.

## Implementation details

- In order to perform group by we will first perform sorting based on the grouping attribute.
- There are two aggregate functions :
    - groupByConditionAggregateFunction - function to perform on which the having clause will be executed and to evaluate the given condition
    - groupByReturnAggregateFunction - function to perform aggregation on the result attribute
- With respect to both the aggregate functions there are two variable to store the result of respective aggregate function
    - conditionValue → value on which the given condition aggregate function will be executed and the result will be stored to evaluate with threshold value
    - returnValue → value which will be added to the resultant row, if the evaluation between the conditionValue and the threshold evaluates to true
- We have implemented a moving aggregation to find the condition value and return value.
- These attributes are first calculated for a specific value of grouping attribute, and when the value of grouping attribute changes the condition value is evaluated with the threshold value and added to the resultant row if it evaluates to true;
- The group by operation is performed in a single pass.

## Assumptions

- If any join or group by operation results in an empty set INT_MIN values will be displayed

## Learnings

- Learned different ways of performing join operation
- learned the difference between brute-force, hash-join and sort merge join method
- understood which sort algorithm to use at what type of data.
- learned the functioning of group by algorithm
- first implemented a brute force method of group by and them improved to the most optimal way using sorting

## Contributions

### Jatin Sharma

- implemented SORT and ORDER BY operation
- optimized the GROUP and ORDER BY algorithm
- worked on few bug fixes during the project implementation

### Harshit Kashyap

- implemeted JOIN and GROUP BY operation
- optimized the JOIN and GROUP BY algorithm
- worked on few bug fixes during the project implementation

### Yash Pathak

- implemeted JOIN and GROUP BY operation
- optimized the ORDER BY algorithm
- did thorough testing
