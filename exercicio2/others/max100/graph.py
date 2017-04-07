import numpy as np
from mpl_toolkits.mplot3d import axes3d
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib import cm
from itertools import *

d = {'gcc' : np.array([[[0.0384,0.1491],[0.0095,0.1443],[0.0040,0.1428],[0.0028,0.1424],[0.0024,0.1422]],[[0.0219,0.1487],[0.0055,0.1436],[0.0023,0.1421],[0.0016,0.1416],[0.0014,0.1414]],[[.0134,0.0802],[0.0034,0.0756],[0.0015,0.0746],[0.0010,0.0743],[0.0009,0.0742]],[[0.0092,0.0450],[0.0025,0.0404],[0.0012,0.0396],[0.0008,0.0394],[0.0006,0.0393]],[[.0072,0.0264],[0.0021,0.0219],[0.0011,0.0212],[0.0007,0.0210],[0.0005,0.0209]]], dtype = np.float64),'mesa' : np.array([[[0.0239,0.0283],[0.0002,0.0287],[0.0000,0.0271],[0.0000,0.0275],[0.0000,0.0275]],[[0.0129,0.0180],[0.0001,0.0177],[0.0000,0.0167],[0.0000,0.0169],[0.0000,0.0168]],[[.0074,0.0104],[0.0001,0.0098],[0.0000,0.0092],[0.0000,0.0094],[0.0000,0.0093]],[[0.0045,0.0086],[0.0000,0.0057],[0.0000,0.0053],[0.0000,0.0054],[0.0000,0.0053]],[[.0045,0.0083],[0.0000,0.0058],[0.0000,0.0033],[0.0000,0.0033],[0.0000,0.0032]]], dtype = np.float64),'galgel' : np.array([[[0.0048,0.3002],[0.0000,0.2746],[0.0000,0.2704],[0.0000,0.2684],[0.0000,0.2680]],[[0.0024,0.3002],[0.0000,0.2746],[0.0000,0.2704],[0.0000,0.2684],[0.0000,0.2680]],[[.0012,0.2617],[0.0000,0.2415],[0.0000,0.2389],[0.0000,0.2377],[0.0000,0.2377]],[[0.0007,0.2429],[0.0000,0.2249],[0.0000,0.2235],[0.0000,0.2234],[0.0000,0.2247]],[[.0004,0.2389],[0.0000,0.2182],[0.0000,0.2158],[0.0000,0.2161],[0.0000,0.2165]]], dtype = np.float64),'art' : np.array([[[0.0002,0.5598],[0.0001,0.5531],[0.0001,0.5523],[0.0001,0.5523],[0.0001,0.5523]],[[0.0001,0.5599],[0.0001,0.5531],[0.0001,0.5524],[0.0001,0.5524],[0.0001,0.5524]],[[.0001,0.4681],[0.0000,0.4536],[0.0000,0.4509],[0.0000,0.4508],[0.0000,0.4508]],[[0.0000,0.4207],[0.0000,0.3974],[0.0000,0.3942],[0.0000,0.3941],[0.0000,0.3942]],[[.0000,0.3914],[0.0000,0.3572],[0.0000,0.3496],[0.0000,0.3488],[0.0000,0.3512]]], dtype = np.float64)}


def plot3d(lgX, lgY, Z, labelZ, nome, interp = 'nearest'):
	lgX,lgY = np.meshgrid(lgX,lgY, indexing='ij')

	fig,ax = plt.subplots()
	heatmap = plt.imshow(Z, interpolation=interp, origin='lower', cmap=cm.gray, extent=[2,6,0,4])


	ax.set_xlabel('lg(Block Size)')
	ax.set_ylabel('lg(Associativity)')

	cbar = plt.colorbar(heatmap)#, shrink=0.8)
	#ax.set_zlabel(labelZ)

	cbar.set_label(labelZ, rotation=270)
	ax.set_title(nome)

	#ax.legend(proxys, labels, prop={'size':10})
	plt.show()
	fig.savefig('figs/heatmap_'+nome+'.png')


'''
def plot2d(X, Y, Z, labelZ, nome):
	n = np.arange(X.shape[0]*Y.shape[0])
	fig, ax = plt.subplots()
	plt.xticks(n, [str(i) + "," + str(j) for i,j in product(X,Y)])
	ax.scatter(n, Z[...,0], c = 'g', label = "LRU")
	ax.scatter(n, Z[...,1], c = 'r', label = "Random")
	ax.scatter(n, Z[...,2], c = 'b', label = "FIFO")
	ax.set_xlabel("lg(Associatividade), lg(Bytes por Bloco)")
	ax.set_ylabel(labelZ)
	ax.set_title(nome)
	plt.show()
	fig.savefig('figs/scatter2d_'+nome+'.png')
'''

lgx = np.arange(5)+2
lgy = np.arange(5)

for nome in d:
	y = 1-d[nome][...,0]
	plot3d(lgx, lgy, y, 'Demand Hit Rate', nome+"_i")
	dy = np.gradient(y)
	dy = np.sqrt(dy[0]**2 + dy[1]**2)
	plot3d(lgx, lgy, dy, 'Demand Hit Rate Change', nome+"_iChange", 'bilinear')
	y = 1-d[nome][...,1]
	plot3d(lgx, lgy, y, 'Demand Hit Rate', nome+"_d")
	dy = np.gradient(y)
	dy = np.sqrt(dy[0]**2 + dy[1]**2)
	plot3d(lgx, lgy, dy, 'Demand Hit Rate Change', nome+"_dChange", 'bilinear')
