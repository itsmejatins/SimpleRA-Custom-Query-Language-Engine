import random
import csv

def generate_random_row(x, d):
    rows = []
    for _ in range(x):
        random_integers = [str(random.randint(10**(d-1), 10**d - 1)) for _ in range(3)]
        row = ",".join(random_integers)
        rows.append(row)
    return rows

# Number of rows to generate
X = 1000  # Change this to generate a different number of rows

# Number of digits for each random integer
D = 3  # Change this to specify a different number of digits

N = 3 # Change this to specify the number of rows in the vector

# Generate random rows
random_rows = generate_random_row(X, D)

# Write the rows to a CSV file
with open('random_data.csv', 'w', newline='') as csvfile:
    csvwriter = csv.writer(csvfile)
    for row in random_rows:
        csvwriter.writerow([row])
        
print("CSV file 'random_data.csv' has been generated.")

