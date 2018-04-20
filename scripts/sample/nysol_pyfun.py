#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import nysol.util.margs as margs
import nysol.take.mitemset as mitemset
import nysol.mod as nm


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
		mitemset.mitemset(args).run()
		dir = o.split("=")[1]
		for line in open(dir+"/patterns.csv"):
			sys.stdout.write(line)

	except Exception as err:
		print(err)
	#extTake.lcmtrans("/tmp/__MTEMP_25238_4411333656_0","p","cfvdf")

	return None

#a=("i=man1.csv","tid=tid","type=F","S=3","O=./vvv")
#print ("aaaa")
f = nm.runfunc(check,"i=man1.csv","tid=tid","type=F","S=3","O=./vvv").mcut(f="pid,size,count,total,support,lift,pattern").run(msg="on")
print(f)

#==============================================================

def check2(v):

	try:
		f   = nm.mstdin()
		f <<= nm.mselstr(f="id",v=v)
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

	

def check4(eF=None,eL=None,t1=None,t2=None,nF=None,nL=None,tl=None):
	print(a)
	try:
		
		sys.stderr.write("st2\n")
		sys.stderr.write("st2 "+ str(eF) +"\n")
		sys.stderr.write("st2 "+ str(eL) +"\n")
		sys.stderr.write("st2 "+ str(t1) +"\n")
		sys.stderr.write("st2 "+ str(t2) +"\n")
		sys.stderr.write("st2 "+ str(nF) +"\n")
		sys.stderr.write("st2 "+ str(nL) +"\n")
		sys.stderr.write("st2 "+ str(tl) +"\n")
	except Exception as err:
		sys.stderr.write(sys.exc_info())	
		sys.stderr.write("\n")


def check5(eF=None,eL=None,t1=None,t2=None,nF=None,nL=None,tl=None):
	print(a)
		
	sys.stderr.write("st2\n")
	sys.stderr.write("st2 "+ str(eF) +"\n")
	sys.stderr.write("st2 "+ str(eL) +"\n")
	sys.stderr.write("st2 "+ str(t1) +"\n")
	sys.stderr.write("st2 "+ str(t2) +"\n")
	sys.stderr.write("st2 "+ str(nF) +"\n")
	sys.stderr.write("st2 "+ str(nL) +"\n")
	sys.stderr.write("st2 "+ str(tl) +"\n")

nm.runfunc(check5,"aaaa",t1="xxx",nF=2).run()

f = nm.mselrand(c=10,i="featureAll.ans.csv")
f <<= nm.mcut(f="id,fldkey,way,val")
f <<= nm.runfunc(check3)
f <<= nm.mcut(f="id,fldkey,way,val",o="xxv0")
f.run(msg="on")


f = nm.mselrand(c=10000,i="featureAll.ans.csv")
f <<= nm.mcut(f="id,fldkey,way,val")
f <<= nm.runfunc(check2,"1")
f <<= nm.mcut(f="id,fldkey,way,val",o="xxv2")
f.run(msg="on")

