#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
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

scp ="""\
readcsv {inpdata}
head -n 10
mcut f=tid,item,val
msum k=tid f=val
writecsv runcheck.csv\
""".format(inpdata=inpname)


if len(sys.argv) != 2:
	print("run sample")
	scpname = temo.file()

	with open(scpname,"w") as wfp:
		wfp.write(scp)

	with open(inpname,"w") as wfp:
		wfp.write(data)

else:
	scpname = sys.argv[1]
	


f=None
with open(scpname,"r") as fp:
	
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