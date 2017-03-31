from __future__ import print_function
import httplib2
import os

from apiclient import discovery
from oauth2client import client
from oauth2client import tools
from oauth2client.file import Storage

try:
	import argparse
	flags = argparse.ArgumentParser(parents=[tools.argparser]).parse_args()
except ImportError:
	flags = None

import numpy as np
np.set_printoptions(threshold=np.nan,linewidth=np.nan, precision=4)
from numpy import *

#import pylab as plt

import matplotlib.pyplot as plt
import matplotlib.cm as cm
from mpl_toolkits.mplot3d import Axes3D
import scipy.stats.mstats as stats

def plotMatrixMap(data, mask, name):
	data[~mask] = nan

	undef = - nanmax(data)/6
	
	data[~mask] = undef

	fig, ax = plt.subplots()

	cax = ax.imshow(data, cmap=plt.cm.rainbow, interpolation='nearest')
	ax.set_title(name)

	cbar = fig.colorbar(cax, ticks=[undef, 0, 1])
	cbar.ax.set_yticklabels(['?', '0', '1'])  # vertically oriented colorbar


	# Move left and bottom spines outward by 10 points
	ax.spines['left'].set_position(('outward', 10))
	ax.spines['bottom'].set_position(('outward', 10))
	# Hide the right and top spines
	ax.spines['right'].set_visible(False)
	ax.spines['top'].set_visible(False)
	# Only show ticks on the left and bottom spines
	ax.yaxis.set_ticks_position('left')
	ax.xaxis.set_ticks_position('bottom')

	ax.set_xlabel('Benchmarks')
	ax.set_ylabel('Máquinas')

	plt.show()

def plotScater2d12(data, mask, name):
	nC, nB = mask.shape
	fig = plt.figure()
	fig.suptitle(name)
	axC = fig.add_subplot(121)
	handles = []
	labels = []
	for c in range(nC):
		bs = (arange(nB)+1)[mask[c,:]]
		ss = data[c, mask[c,:]]
		handle, = axC.plot(bs, ss, '.r-', color = cm.rainbow(c/nC), label = "Máquina %d" % (c+1))
		handles += [handle]

	axC.set_xlabel('Benchmarks')
	axC.set_ylabel('Scores')
	axC.set_title("Scores per Benchmarks")

	plt.legend(handles= handles, prop={'size':10})

	axB = fig.add_subplot(122)

	handles = []
	labels = []
	for b in range(nB):
		cs = (arange(nC)+1)[mask[:,b]]
		ss = data[mask[:,b], b]
		handle, = axB.plot(cs, ss, '.r-', color = cm.rainbow(b/nB), label = "Benchmark %d" % (b+1))
		handles += [handle]

	axB.set_xlabel('Máquinas')
	axB.set_ylabel('Scores')
	axB.set_title("Scores per Máquinas")

	plt.legend(handles= handles)

	plt.show()

def plotScater2d1(data, mask, name):
	nC, nB = mask.shape
	fig, ax = plt.subplots()
	handles = []
	labels = []
	for c in range(nC):
		bs = (arange(nB)+1)[mask[c,:]]
		ss = data[c, mask[c,:]]
		handle, = ax.plot(bs, ss, '.r-', color = cm.rainbow(c/nC), label = "Máquina %d" % (c+1))
		handles += [handle]

	ax.set_xlabel('Benchmarks')
	ax.set_ylabel('Scores')
	ax.set_title(name)

	plt.legend(handles= handles, prop={'size':10})

	plt.show()

def plotScater2d2(data, mask, name):
	nC, nB = mask.shape
	fig, ax = plt.subplots()
	handles = []
	labels = []
	for b in range(nB):
		cs = (arange(nC)+1)[mask[:,b]]
		ss = data[mask[:,b], b]
		handle, = ax.plot(cs, ss, '.r-', color = cm.rainbow(b/nB), label = "Benchmark %d" % (b+1))
		handles += [handle]

	ax.set_xlabel('Máquinas')
	ax.set_ylabel('Scores')
	ax.set_title(name)

	plt.legend(handles= handles)

	plt.show()

def plotScater3d(data, mask, name):
	nC, nB = mask.shape

	fig = plt.figure()
	ax = fig.add_subplot(111, projection='3d')

	for c in range(nC):
		cs = full(shape=(nB), fill_value=c+1)[mask[c,:]]
		bs = (arange(nB)+1)[mask[c,:]]
		ss = data[c, mask[c,:]]
		ax.bar(cs, bs, ss, zdir='y', color = cm.rainbow(c/nC))

	ax.set_xlabel('Máquinas')
	ax.set_ylabel('Benchmarks')
	ax.set_zlabel('Scores')

	plt.show()


def normalize1(data, mask):
	nC, nB = mask.shape
	data[~mask] = nan
	bmax = nanmax(data, axis = 0)
	bmin = nanmin(data, axis = 0)

	print(bmax)
	print(bmin)
	#Initial adjustments
	data -= kron(bmin, ones((nC, 1)))

	data /= kron((bmax - bmin), ones((nC,1)))

	return data, mask

def normalize2(data, mask):
	(nC, nB) = mask.shape
	handled = full(shape = (nB), fill_value = False, dtype = np.bool)
	print("Começa a normalizar: \n\n")
	for c in range(0,nC):
		if(all(handled)):
			break
		factor = stats.gmean(data[c, handled]) if any(handled) else 1
		print(" Handled: ", handled)
		handle = mask[c,:] & ~handled
		print("Handling: ", handle)
		if(any(handle)):
			data[:,handle] *= kron(factor/data[c,handle], ones((nC, 1)))
			handled = handled | handle
			

	return data, mask

	print(data)

def plotMatrix(data):
	print(data[0][0])
	arr = array(data)
	print(data)
	nC = len(data)
	nB = 13
	print(nC, nB)
	scores = empty(shape = (nC, nB), dtype = float64)
	gotten = empty(shape = (nC, nB),  dtype = np.bool)

	i = 0
	for line in data:
		j = 0
		for cell in line:
			if cell == '':
				scores[i,j] = nan
				gotten[i,j] = False
			else:
				scores[i,j] = float64(cell)
				gotten[i,j] = True
			j+=1

		i+=1

	print(scores)

	plotScater2d1(scores, gotten, "Scores X Benchmarks X Máquinas")
	plotScater2d2(scores, gotten, "Scores X Máquinas X Benchmarks")
	plotScater3d(scores, gotten, "Scores X Benchmarks X Máquinas")
	plotMatrixMap(scores, gotten, "Scores X Benchmarks X Máquinas")
	'''
	scores = float64(arr)
	print(scores)
	'''


# If modifying these scopes, delete your previously saved credentials
# at ~/.credentials/sheets.googleapis.com-python-quickstart.json
SCOPES = 'https://www.googleapis.com/auth/spreadsheets.readonly'
CLIENT_SECRET_FILE = 'client_secret.json'
APPLICATION_NAME = 'Google Sheets API Python Quickstart'


def get_credentials():
	"""Gets valid user credentials from storage.

	If nothing has been stored, or if the stored credentials are invalid,
	the OAuth2 flow is completed to obtain the new credentials.

	Returns:
		Credentials, the obtained credential.
	"""
	home_dir = os.path.expanduser('~')
	credential_dir = os.path.join(home_dir, '.credentials')
	if not os.path.exists(credential_dir):
		os.makedirs(credential_dir)
	credential_path = os.path.join(credential_dir,
								   'sheets.googleapis.com-python-quickstart.json')

	store = Storage(credential_path)
	credentials = store.get()
	if not credentials or credentials.invalid:
		flow = client.flow_from_clientsecrets(CLIENT_SECRET_FILE, SCOPES)
		flow.user_agent = APPLICATION_NAME
		if flags:
			credentials = tools.run_flow(flow, store, flags)
		else: # Needed only for compatibility with Python 2.6
			credentials = tools.run(flow, store)
		print('Storing credentials to ' + credential_path)
	return credentials

def getSpreadSheet(spreadsheetId, range):
	credentials = get_credentials()
	http = credentials.authorize(httplib2.Http())
	discoveryUrl = ('https://sheets.googleapis.com/$discovery/rest?'
					'version=v4')
	service = discovery.build('sheets', 'v4', http=http,
							  discoveryServiceUrl=discoveryUrl)

	rangeName = 'eq1!C3:O52'
	result = service.spreadsheets().values().get(
		spreadsheetId=spreadsheetId, range=rangeName).execute()
	values = result.get('values', [])

	if not values:
		print('No data found.')
		return "No data found"
	else:
		return values

def main():
	val = getSpreadSheet('1bHE_ILLOjKkYOd6otzc_2iIkMJ4Uw27h8d58YBZLLO4', 'eq1!C3:O52')
	plotMatrix(val)
	

if __name__ == '__main__':
	main()