import numpy as np
from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt
from matplotlib import cm
from itertools import *


d = {'gcc' : np.array([[[0.4922,0.1732],[0.4922,0.1302],[0.4922,0.1024],[0.4922,0.0762],[0.4922,0.0531],[0.4922,0.0321],[0.4922,0.0227],[0.4922,0.0156],[0.4922,0.0095]],[[0.4003,0.1732],[0.4003,0.1302],[0.4003,0.1024],[0.4003,0.0762],[0.4003,0.0531],[0.4003,0.0321],[0.4003,0.0227],[0.4003,0.0156],[0.4003,0.0095]],[[0.2944,0.1732],[0.2944,0.1302],[0.2944,0.1024],[0.2944,0.0762],[0.2944,0.0531],[0.2944,0.0321],[0.2944,0.0227],[0.2944,0.0156],[0.2944,0.0095]],[[0.1712,0.1732],[0.1712,0.1302],[0.1712,0.1024],[0.1712,0.0762],[0.1712,0.0531],[0.1712,0.0321],[0.1712,0.0227],[0.1712,0.0156],[0.1712,0.0095]],[[0.0952,0.1732],[0.0952,0.1302],[0.0952,0.1024],[0.0952,0.0762],[0.0952,0.0531],[0.0952,0.0321],[0.0952,0.0227],[0.0952,0.0156],[0.0952,0.0095]],[[0.0593,0.1732],[0.0593,0.1302],[0.0593,0.1024],[0.0593,0.0762],[0.0593,0.0531],[0.0593,0.0321],[0.0593,0.0227],[0.0593,0.0156],[0.0593,0.0095]],[[0.0132,0.1732],[0.0132,0.1302],[0.0132,0.1024],[0.0132,0.0762],[0.0132,0.0531],[0.0132,0.0321],[0.0132,0.0227],[0.0132,0.0156],[0.0132,0.0095]],[[0.0095,0.1732],[0.0095,0.1302],[0.0095,0.1024],[0.0095,0.0762],[0.0095,0.0531],[0.0095,0.0321],[0.0095,0.0227],[0.0095,0.0156],[0.0095,0.0095]],[[0.0025,0.1732],[0.0025,0.1302],[0.0025,0.1024],[0.0025,0.0762],[0.0025,0.0531],[0.0025,0.0321],[0.0025,0.0227],[0.0025,0.0156],[0.0025,0.0095]]]),'mesa' : np.array([[[0.3960,0.0493],[0.3960,0.0201],[0.3960,0.0074],[0.3960,0.0047],[0.3960,0.0033],[0.3960,0.0033],[0.3960,0.0033],[0.3960,0.0033],[0.3960,0.0033]],[[0.2795,0.0493],[0.2795,0.0201],[0.2795,0.0074],[0.2795,0.0047],[0.2795,0.0033],[0.2795,0.0033],[0.2795,0.0033],[0.2795,0.0033],[0.2795,0.0033]],[[0.1611,0.0493],[0.1611,0.0201],[0.1611,0.0074],[0.1611,0.0047],[0.1611,0.0033],[0.1611,0.0033],[0.1611,0.0033],[0.1611,0.0033],[0.1611,0.0033]],[[0.0692,0.0493],[0.0692,0.0201],[0.0692,0.0074],[0.0692,0.0047],[0.0692,0.0033],[0.0692,0.0033],[0.0692,0.0033],[0.0692,0.0033],[0.0692,0.0033]],[[0.0456,0.0493],[0.0456,0.0201],[0.0456,0.0074],[0.0456,0.0047],[0.0456,0.0033],[0.0456,0.0033],[0.0456,0.0033],[0.0456,0.0033],[0.0456,0.0033]],[[0.0001,0.0493],[0.0001,0.0201],[0.0001,0.0074],[0.0001,0.0047],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033]],[[0.0001,0.0493],[0.0001,0.0201],[0.0001,0.0074],[0.0001,0.0047],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033]],[[0.0001,0.0493],[0.0001,0.0201],[0.0001,0.0074],[0.0001,0.0047],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033]],[[0.0001,0.0493],[0.0001,0.0201],[0.0001,0.0074],[0.0001,0.0047],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033],[0.0001,0.0033]]]),'galgel' : np.array([[[0.0241,0.0298],[0.0241,0.0254],[0.0241,0.0213],[0.0241,0.0189],[0.0241,0.0175],[0.0241,0.0153],[0.0241,0.0140],[0.0241,0.0137],[0.0241,0.0136]],[[0.0005,0.0298],[0.0005,0.0254],[0.0005,0.0213],[0.0005,0.0189],[0.0005,0.0175],[0.0005,0.0153],[0.0005,0.0140],[0.0005,0.0137],[0.0005,0.0136]],[[0.0004,0.0298],[0.0004,0.0254],[0.0004,0.0213],[0.0004,0.0189],[0.0004,0.0175],[0.0004,0.0153],[0.0004,0.0140],[0.0004,0.0137],[0.0004,0.0136]],[[0.0001,0.0298],[0.0001,0.0254],[0.0001,0.0213],[0.0001,0.0189],[0.0001,0.0175],[0.0001,0.0153],[0.0001,0.0140],[0.0001,0.0137],[0.0001,0.0136]],[[0.0001,0.0298],[0.0001,0.0254],[0.0001,0.0213],[0.0001,0.0189],[0.0001,0.0175],[0.0001,0.0153],[0.0001,0.0140],[0.0001,0.0137],[0.0001,0.0136]],[[0.0001,0.0298],[0.0001,0.0254],[0.0001,0.0213],[0.0001,0.0189],[0.0001,0.0175],[0.0001,0.0153],[0.0001,0.0140],[0.0001,0.0137],[0.0001,0.0136]],[[0.0001,0.0298],[0.0001,0.0254],[0.0001,0.0213],[0.0001,0.0189],[0.0001,0.0175],[0.0001,0.0153],[0.0001,0.0140],[0.0001,0.0137],[0.0001,0.0136]],[[0.0001,0.0298],[0.0001,0.0254],[0.0001,0.0213],[0.0001,0.0189],[0.0001,0.0175],[0.0001,0.0153],[0.0001,0.0140],[0.0001,0.0137],[0.0001,0.0136]],[[0.0001,0.0298],[0.0001,0.0254],[0.0001,0.0213],[0.0001,0.0189],[0.0001,0.0175],[0.0001,0.0153],[0.0001,0.0140],[0.0001,0.0137],[0.0001,0.0136]]]),'art' : np.array([[[0.0076,0.5524],[0.0076,0.5432],[0.0076,0.5394],[0.0076,0.5364],[0.0076,0.5355],[0.0076,0.4930],[0.0076,0.4652],[0.0076,0.4612],[0.0076,0.3405]],[[0.0007,0.5524],[0.0007,0.5432],[0.0007,0.5394],[0.0007,0.5364],[0.0007,0.5355],[0.0007,0.4930],[0.0007,0.4652],[0.0007,0.4612],[0.0007,0.3405]],[[0.0006,0.5524],[0.0006,0.5432],[0.0006,0.5394],[0.0006,0.5364],[0.0006,0.5355],[0.0006,0.4930],[0.0006,0.4652],[0.0006,0.4612],[0.0006,0.3405]],[[0.0000,0.5524],[0.0000,0.5432],[0.0000,0.5394],[0.0000,0.5364],[0.0000,0.5355],[0.0000,0.4930],[0.0000,0.4652],[0.0000,0.4612],[0.0000,0.3405]],[[0.0000,0.5524],[0.0000,0.5432],[0.0000,0.5394],[0.0000,0.5364],[0.0000,0.5355],[0.0000,0.4930],[0.0000,0.4652],[0.0000,0.4612],[0.0000,0.3405]],[[0.0000,0.5524],[0.0000,0.5432],[0.0000,0.5394],[0.0000,0.5364],[0.0000,0.5355],[0.0000,0.4930],[0.0000,0.4652],[0.0000,0.4612],[0.0000,0.3405]],[[0.0000,0.5524],[0.0000,0.5432],[0.0000,0.5394],[0.0000,0.5364],[0.0000,0.5355],[0.0000,0.4930],[0.0000,0.4652],[0.0000,0.4612],[0.0000,0.3405]],[[0.0000,0.5524],[0.0000,0.5432],[0.0000,0.5394],[0.0000,0.5364],[0.0000,0.5355],[0.0000,0.4930],[0.0000,0.4652],[0.0000,0.4612],[0.0000,0.3405]],[[0.0000,0.5524],[0.0000,0.5432],[0.0000,0.5394],[0.0000,0.5364],[0.0000,0.5355],[0.0000,0.4930],[0.0000,0.4652],[0.0000,0.4612],[0.0000,0.3405]]])}

def plot3d(lgX, lgY, Z, nome):
	lgX,lgY = np.meshgrid(lgX,lgY)

	fig = plt.figure()
	ax = fig.gca(projection='3d')
	labels = ("Instruction Cache Demand Miss Rate", "Data Cache Demand Miss Rate")
	proxys = []
	for i, c, cmap_name in zip((0,1),('r', 'g'),('Reds', 'Greens')):
		ax.plot_surface(lgX, lgY, Z[...,i], color = c, alpha=0.3)
		proxy = plt.Rectangle((0, 0), 1, 1, fc=c)
		proxys += [proxy]

		cset = ax.contour(lgX, lgY, Z[...,i], zdir='z',  cmap=plt.get_cmap(cmap_name))
		cset = ax.contour(lgX, lgY, Z[...,i], zdir='x',  cmap=plt.get_cmap(cmap_name))
		cset = ax.contour(lgX, lgY, Z[...,i], zdir='y',  cmap=plt.get_cmap(cmap_name))

	'''
	cset = ax.contour(lgX, lgY, Z, zdir='z', offset=-100, cmap=cm.coolwarm)
	cset = ax.contour(lgX, lgY, Z, zdir='x', offset=-40, cmap=cm.coolwarm)
	cset = ax.contour(lgX, lgY, Z, zdir='y', offset=40, cmap=cm.coolwarm)
	'''
	ax.set_xlabel('lg(Data Cache Size)')
	ax.set_ylabel('lg(Instruction Cache Size)')
	ax.set_zlabel('Demand Miss Rate')
	ax.set_title(nome)

	ax.legend(proxys, labels, prop={'size':10})

	plt.show()
	fig.savefig('figs/'+nome+'.png')

lgi = lgd = np.arange(9)+11

for nome in d:
	plot3d(lgi, lgd, d[nome], "size_"+nome)
	z_d = np.diag(d[nome][...,0])
	z_i = np.diag(d[nome][...,1])
	plt.figure()
	plt.plot(lgd, z_d)

