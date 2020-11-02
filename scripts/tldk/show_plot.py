from datetime import datetime
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from io import StringIO

f = open('./plot-curve')
csv = 'time,old_cov,new_cov\n'
count = 0
for line in f:
    count += 1
    line_items = line.split(' ')
    # print(line_items)
    time = line_items[0]
    old_cov = line_items[3][0:line_items[3].index('(')]
    new_cov = line_items[12][3:line_items[12].index('(')]

    # old_cov = line_items[3][0:line_items[3].index('(')]
    # new_cov = line_items[12]
    # csv += time + ',' + old_cov + ',' + '0' + '\n'
    csv += time + ',' + old_cov + ',' + new_cov + '\n'
    # if count > 1000:
    #     break

# print(csv)
data = pd.read_csv(StringIO(csv))

# for i in data:
#     print(i)
#     break

df = pd.DataFrame(data, columns = ['time', 'old_cov', 'new_cov'])

# print(df)
# Set the Date as Index
# df['time'] = pd.to_datetime(df['time'], unit='us')
df['time'] = pd.to_timedelta(df['time'] - df['time'][0], unit='us')
df.index = df['time']
del df['time']

# df['old_cov'] = np.log10(df['old_cov'])
# df['new_cov'] = np.log10(df['new_cov'])

# print(df)

df.plot(figsize=(15, 6))
plt.savefig('test.png')