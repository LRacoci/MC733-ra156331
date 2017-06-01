import numpy as np

inp = '''
1 0 2 3 5 4 6 8 1
1 2 3 5 6 4 8 2 2
1 2 5 6 2 5 2 5 2
9 5 6 5 6 8 5 1 4
6 9 8 5 2 3 5 3 1
5 2 3 6 3 6 8 7 2
1 2 3 6 2 5 5 9 2
9 5 6 5 6 8 5 1 4
1 2 3 5 6 4 8 2 2
'''
def get_array(inp, n):
	inp = inp.replace('\n', ' ')
	inp = inp.split(' ')
	inp = [int(x) for x in inp if x != '']
	inp = np.array(inp)
	inp.shape = (n,n)

inp = '''
1 0 2 3 5 4 6 8 1
1 2 3 5 6 4 8 2 2
1 2 5 6 2 5 2 5 2
9 5 6 5 6 8 5 1 4
6 9 8 5 2 3 5 3 1
5 2 3 6 3 6 8 7 2
1 2 3 6 2 5 5 9 2
9 5 6 5 6 8 5 1 4
1 2 3 5 6 4 8 2 2
'''
inp = inp.replace('\n', ' ')
inp = inp.split(' ')
inp = [int(x) for x in inp if x != '']
inp = np.array(inp)
inp.shape = (n,n)
out = '''
      1      0      2      3      5      4      6      8      1
      6     10     23     31     23     27     28     27     11
     62     44     69     73     63     85     62     43     39
    101    101    141    143    153    192    225    191     73
    185    127    165    186    154    216    194    132     97
    159    140    198    246    193    241    287    245     95
    135    127    155    178    117    173    181    156     73
     95     83    134    164    143    164    183    138     78
    183    130    175    207    161    232    220    195     98
'''


out = out.replace('\n', ' ')
out = out.split(' ')
out = [int(x) for x in out if x != '']
out = np.array(out)
out.shape = (n,n)