#kiddtojanu.py
f = open('kidd26.txt', 'rw')

a = f.read()
for i in a:
	for c in i:
		if( i != '\n' and i != ' '):
			if(ord(c) >= ord('a')):
				print ord(c) -  ord('a') + 10,
			else:
				print c,
		else:
			print c,