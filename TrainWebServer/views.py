from django.http import *
import subprocess
from os.path import *
import os
import time

TMP_DIR = '/tmp/trainserver/'

def lock_file(name):
	while os.path.isfile(name + '.lock'):
		print 'waiting for lock'
	f = open(name + '.lock', 'w')
	f.close()

def unlock_file(name):
	assert(os.path.isfile(name + '.lock'))
	os.remove(name + '.lock')

def tranMessage(request, count):
	if not 'city1' in request.GET:
		return 
	else:
		city1 = request.GET['city1']
	if not 'city2' in request.GET:
		return 
	else:
		city2 = request.GET['city2']
	if not 'need' in request.GET:
		return 
	else:
		need = request.GET['need']

	if not 'change_limit' in request.GET or request.GET['change_limit'] == "":
		change_limit = -1
	else:
		change_limit = request.GET['change_limit']
	if not 'late' in request.GET or request.GET['late'] == "":
		late = 0
	else:
		late = request.GET['late']
	if not 'rate' in request.GET or request.GET['rate'] == "":
		rate = 0
	else:
		rate = request.GET['rate']
	if not 'earliest' in request.GET or request.GET['earliest'] == "":
		earliest = 0
	else:
		earliest = request.GET['earliest']
	if not 'latest' in request.GET or request.GET['latest'] == "":
		latest = 24*60 - 1
	else:
		latest = request.GET['latest']

	fname = TMP_DIR + str(count) + '.query'
	lock_file(fname)
	f = open(fname, 'w')
	argv = '%s %s %s %s %s %s %s %s' % (city1, city2, change_limit, late, need, earliest, latest, rate)
	print argv
	f.write(argv.encode('utf8'))
	f.close()
	unlock_file(fname)
	fname = TMP_DIR + str(count) + '.finish'
	while not os.path.isfile(fname):
		print 'waiting for calculation %d' % (count)
		time.sleep(1)


def query_plan(request):
	fname = TMP_DIR + 'count.txt'
	lock_file(fname)
	f = open(fname, 'r')
	count = int(f.read()) + 1
	print 'count now is %d' % (count)
	f.close()
	f = open(fname, 'w')
	f.write(str(count))
	f.close()
	unlock_file(fname)

	tranMessage(request, count)
	
	fname = TMP_DIR + str(count) + '.result'
	f = open(fname, 'r')
	ret = f.read()
	f.close()

	return HttpResponse(ret)

