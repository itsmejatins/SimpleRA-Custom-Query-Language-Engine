import pandas as pd
import numpy as np
import random

# Generate Table 1
table1 = pd.DataFrame({
    'ID1': np.random.randint(1,100,100),
    'Attribute1': np.random.randint(1, 100, 100),
    'Attribute2': np.random.randint(1, 100, 100)
})

# Generate Table 2
table2 = pd.DataFrame({
    'ID2': np.random.randint(1,100,100),
    'Attribute3': np.random.randint(1, 100, 100),
    'Attribute4': np.random.randint(1, 100, 100)
})

table1 = table1.sort_values(by='ID1')
table2 = table2.sort_values(by='ID2')

# Save tables to CSV files
table1.to_csv('TABLE1.csv', index=False)
table2.to_csv('TABLE2.csv', index=False)


# Read the CSV files back into DataFrames
table1 = pd.read_csv('table1.csv')
table2 = pd.read_csv('table2.csv')

table1 = table1.sort_values(by='ID1')
table2 = table2.sort_values(by='ID2')

# Perform joins
result = pd.merge(table1, table2, left_on='ID1', right_on='ID2', how='inner', suffixes=('_table1','_table2'))  # Inner join
# result = table1.merge(table2[table1['ID1'] >= table2['ID2']], how='inner', suffixes=('_table1','_table2'))
# result = table1.merge(table2[table2['ID2'] >= table1['ID1']],left_on='ID1', right_on='ID2', how='inner',suffixes=('_table1','_table2'))





result.to_csv('RESULT.csv', index=False)

