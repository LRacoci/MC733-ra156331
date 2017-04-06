import numpy as np
from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt
from matplotlib import cm
from itertools import *


d = {'gcc' : np.array([[0.3531,0.2430],[0.2787,0.2147],[0.2008,0.1963],[0.1188,0.1780],[0.0654,0.1624],[0.0384,0.1491]]),'mesa' : np.array([[0.5273,0.1516],[0.3882,0.1070],[0.1731,0.0551],[0.0630,0.0382],[0.0239,0.0283],[0.0065,0.0221]]),'galgel' : np.array([[0.0184,0.4509],[0.0048,0.3313],[0.0048,0.3002],[0.0001,0.2757],[0.0000,0.2502],[0.0000,0.2358]]),'art' : np.array([[0.0011,0.5743],[0.0002,0.5598],[0.0001,0.5561],[0.0000,0.5542],[0.0000,0.5532],[0.0000,0.5101]])}

def simplePlot(lgX, labelx, Y, labely, nome):
	fig,ax = plt.subplots()

	handles = []
	for i, c, label in zip((0,1),('r', 'g'),('Instruction', 'Data')):
		handle, = ax.plot(lgX, Y[...,i], color = c, label = label)
		handles += [handle]

	fig.suptitle(nome, fontsize=20)
	plt.xlabel(labelx, fontsize=18)
	plt.ylabel(labely, fontsize=18)

	plt.legend(handles= handles, prop={'size':10})
	plt.show()
	fig.savefig('figs/'+nome+'.png')

lgs = np.arange(6)+11

for nome in d:
	y = 1 - d[nome]
	simplePlot(lgs, 'lg(Data and Instruction Cache Size)' , y, 'Demand Hit Rate',  nome)
	# Forward derivative, enphasis on the first, copy the last
	dy = np.append(y[1:], y[-1]).reshape(y.shape) - y
	simplePlot(lgs, 'lg(Data and Instruction Cache Size)', dy, 'Demand Hit Rate Change', nome+'Change')

