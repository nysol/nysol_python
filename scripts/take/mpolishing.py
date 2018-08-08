#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import re
import nysol.util.margs as margs
import nysol.take.graph as ntg
import nysol.take as nt

args=margs.Margs(sys.argv,"ni=,nf=,ei=,ef=,-indirect,eo=,no=,th=,sim=,sup=,iter=,log=,O=","ei=,ef=,th=")

ei = args.str("ei=")
ni = args.str("ni=",None)
ef = args.str("ef=",None,",")
nf = args.str("nf=",None)
if len(ef) != 2 :
	raise Exception("ef size is 2")

gi=ntg.graph(edgeFile=ei,title1=ef[0],title2=ef[1],nodeFile=ni,title=nf)
para={}
para["gi"]=gi

for k,v in args.keyValue.items():

	if k == "ei=" or k == "ef=" or k == "nf=" or k == "ni=" or k == "eo=" or k == "no=" :
		continue
	if re.match(r'^-',k):
		para[re.sub(r'^-',"",k)]=v
	else:	
		para[re.sub(r'=$',"",k)]=v

gobj = nt.mpolishing(**para).run()

ofile_e =args.str("eo=")
ofile_n =args.str("no=")

if ofile_e:
	ro = gobj.edges()
	ro.writecsv(o=ofile_e).run()

if ofile_n:
	ro = gobj.nodes()
	ro.writecsv(o=ofile_n).run()


	

