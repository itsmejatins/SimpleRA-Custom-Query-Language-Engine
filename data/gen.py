import csv
import random

# Generate data for the CSV
num_rows = 100
num_columns = 100

def getRandomNumber():
    return random.getrandbits(8)

data = []
for i in range(num_rows):
    # row = [getRandomNumber()] * num_columns
    row = [i+1] * num_columns
    data.append(row)


# Write the data to a CSV file
csv_filename = "mat1.csv"
with open(csv_filename, "w", newline="") as csvfile:
    csv_writer = csv.writer(csvfile)
    
    # Write data rows
    csv_writer.writerows(data)

print(f"CSV file '{csv_filename}' has been generated.")


# ***************************************************************************
# ALL RANDOM
# ***************************************************************************


# import random
# import csv

# n = 20  # Change this to the number of rows and columns you want

# # Generate random data
# data = [[random.randint(1, 100) for _ in range(n)] for _ in range(n)]

# # Specify the CSV file name
# csv_filename = "mat2.csv"

# # Write the data to the CSV file
# with open(csv_filename, mode='w', newline='') as file:
#     writer = csv.writer(file)
#     for row in data:
#         writer.writerow(row)

# print(f"CSV file '{csv_filename}' generated with {n} rows and {n} columns.")