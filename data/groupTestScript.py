import csv
import random

# Number of rows for the CSV
num_rows = 100  # You can change this to the desired number of rows

# Generate random data and write to CSV
data = []

for i in range(1, num_rows + 1):
    employee_id = i
    department_id = random.randint(1, 20)  # Random department ID between 1 and 4
    salary = random.randint(20000, 100000)  # Random salary between 20,000 and 100,000
    data.append([employee_id, department_id, salary])

# Sort the data by Department_ID
# data.sort(key=lambda x: x[1])

# Write the sorted data to the CSV file
with open('EMPLOYEEGROUP1.csv', 'w', newline='') as file:
    writer = csv.writer(file)
    
    # Write the header row
    writer.writerow(['EMPLOYEEID', 'DEPARTMENT_ID', 'SALARY'])
    
    # Write the sorted data rows
    for row in data:
        writer.writerow(row)

print(f"CSV file 'EMPLOYEEGROUP1.csv' has been generated with {num_rows} random rows sorted by Department_ID.")
