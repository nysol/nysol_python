#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import nysol.util.margs as margs
import nysol.take as nt
import nysol.mcmd as nm
import nysol.util.mtemp as mtemp

data = """\
tid,item,val
T1,C,3
T1,E,5
T2,D,12
T2,E,3
T2,F,4
T3,A,5
T3,B,6
T3,D,7
T3,F,8
T4,B,7
T4,D,1
T4,F,2
T5,A,44
T5,B,5
T5,D,7
T5,E,8
T6,A,3
T6,B,2
T6,D,1
T6,E,9
T6,F,1\
"""

temo = mtemp.Mtemp()
inpname = temo.file()

with open(inpname,"w") as wfp:
	wfp.write(data)

def check(inf,tid,item,s,o):

	argv=['cmdname']
	argv.append(inf)
	argv.append(tid)
	argv.append(item)
	argv.append(s)
	argv.append(o)

	args=margs.Margs(argv,"i=,x=,O=,tid=,item=,class=,taxo=,type=,s=,S=,sx=,Sx=,g=,p=,-uniform,l=,u=,top=,T=,-replaceTaxo")
	try:
		#print(dir(mitemset))
		nt.mitemset(**(args.kvmap())).run()
		dir = o.split("=")[1]
		for line in open(dir+"/patterns.csv"):
			sys.stdout.write(line)

	except Exception as err:
		print(err)
	#extTake.lcmtrans("/tmp/__MTEMP_25238_4411333656_0","p","cfvdf")

	return None

f = nm.runfunc(check,"i={}".format(inpname),"tid=tid","type=F","S=3","O=./vvv").mcut(f="pid,size,count,total,support,lift,pattern").run(msg="on")
print(f)
#==============================================================

def check2(v):

	try:
		f   = nm.mstdin()
		f <<= nm.mselstr(f="val",v=v)
		f <<= nm.mstdout()
		f.run(msg="on")
	except Exception as err:
		sys.stderr.write("st aaa3\n")
		sys.stderr.write(sys.exc_info())
		sys.stderr.write("st aaa4\n")

def check3():
	import fileinput
	for line in fileinput.input():
		tokens = line.strip().split()
		print(",".join(tokens))

	

f = nm.mselrand(c=50,i=inpname)
f <<= nm.mcut(f="tid,item,val")
f <<= nm.runfunc(check3)
f <<= nm.mcut(f="tid,item,val")
print(f.run(msg="on"))

f = nm.mselrand(c=50,i=inpname)
f <<= nm.mcut(f="tid,item,val")
f <<= nm.runfunc(check2,"1")
f <<= nm.mcut(f="tid,item,val")
print(f.run(msg="on"))


