#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import nysol.mcmd as nm
if len(sys.argv) != 2:
	print("script fileがない")
	exit()

f=None
with open(sys.argv[1],"r") as fp:
	
	lin = fp.readline() 
 
	while lin:
		cmds = lin.rstrip().split(" ",1)
		try :
			xx = getattr(nm,cmds[0])
			para = cmds[1]
		except:
			xx =  nm.cmd
			para = lin.rstrip()

		f <<= xx(para)

		lin = fp.readline()	

f.run(msg="on")