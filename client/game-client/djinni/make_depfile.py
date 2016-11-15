import sys
import os

inlistfile, outpfile, target = sys.argv[1:]

with open(outpfile, 'wb') as outp:
	outp.write(target + ':')
	with open(inlistfile, 'rb') as inp:
		for line in inp:
			line = line.strip()
			if line != target:
				outp.write(' ' + line.replace(' ', '\\ '))

os.remove(inlistfile)
