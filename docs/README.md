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

### **4. Learnings** <br/>
- Gained insight as to how to store matrix data in a block efficiently. Also got more expose as to how to store any type of data efficiently with respect to storing and retrieving.

### **5. Contribution** <br/>

- Jatin Sharma

- Harshit Kashyap

- Yash Pathak


