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
np.set_printoptions(threshold=np.nan)
from numpy import *

def rank(data):
	data = array(data)
	indices = array(data[1:, :2], dtype = uint32)
	
	(nC, nB) = amax(indices, axis = 0)

	scores = empty(shape = (nC, nB), dtype = float64)
	gotten = full(shape = (nC, nB), fill_value = False, dtype = np.bool)
	
	for k in range(1, len(data)):
		scores[uint32(data[k,0])-1, uint32(data[k,1])-1] = float64(data[k,2])
		gotten[uint32(data[k,0])-1, uint32(data[k,1])-1] = True
	
	print(np.sum(gotten, axis = 1))

	"""
	
	alpha = array(nB, dtype = float64)

	for comp in scores:
		for j in range(len(comp)):


	"""



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

def main():
	"""Shows basic usage of the Sheets API.

	Creates a Sheets API service object and prints some info:
	https://docs.google.com/spreadsheets/d/1bHE_ILLOjKkYOd6otzc_2iIkMJ4Uw27h8d58YBZLLO4/edit
	"""
	credentials = get_credentials()
	http = credentials.authorize(httplib2.Http())
	discoveryUrl = ('https://sheets.googleapis.com/$discovery/rest?'
					'version=v4')
	service = discovery.build('sheets', 'v4', http=http,
							  discoveryServiceUrl=discoveryUrl)

	spreadsheetId = '1bHE_ILLOjKkYOd6otzc_2iIkMJ4Uw27h8d58YBZLLO4'
	rangeName = 'Medidas Completas!A:D'
	result = service.spreadsheets().values().get(
		spreadsheetId=spreadsheetId, range=rangeName).execute()
	values = result.get('values', [])

	if not values:
		print('No data found.')
	else:
		rank(values)

if __name__ == '__main__':
	main()