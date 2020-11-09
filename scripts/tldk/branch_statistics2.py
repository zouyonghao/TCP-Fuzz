from datetime import datetime
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from io import StringIO

f = open('./build_log')
function_name = ''
branch_id = ''
branch_function_dict = dict()

for line in f:
    if 'Instrument function' in line:
        line_items = line[0:-1].split(' ')
        # print(line_items)
        function_name = line_items[-1]
        # print(function_name)
    if 'branch Id is' in line:
        line_items = line[0:-1].split(' ')
        # print(line_items)
        branch_id = line_items[-1]
        # print(branch_id)
        branch_function_dict[branch_id] = function_name

# print(branch_function_dict)

f = open('./change_log2')
csv = 'branch_id\n'
count = 0
for line in f:
    count += 1
    line_items = line.split(' ')
    if line_items[0] != 'Change':
        continue
    if len(line_items) < 5:
        continue
    # print(line_items)
    if '\n' not in line_items[4]:
        continue
    br = line_items[4][0:line_items[4].index('\n')]
    csv += branch_function_dict[br] + '\n'
    # if count > 20000:
    #     break
    # break

# print(csv)
data = pd.read_csv(StringIO(csv))

# for i in data:
#     print(i)
#     break

df = pd.DataFrame(data, columns = ['branch_id'])

pd.set_option('display.max_rows', df.shape[0]+1)

df = df.branch_id.value_counts().sort_index()

print(df)