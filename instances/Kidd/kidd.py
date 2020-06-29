f = open('kidd8.txt', 'rw')

a = f.readline()
for a in f:
	for i in a:
		if (i==' '):
			print
		else:
			print i,