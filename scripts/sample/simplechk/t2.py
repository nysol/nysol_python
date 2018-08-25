#!/usr/bin/env python
# -*- coding: utf-8 -*-
import nysol.mcmd as nm
import nysol.util.mtemp as mtemp
import os

sampDAT1="""\
a,b,c
1,2,3
4,5,6
7,8,9
"""

temo = mtemp.Mtemp()
vv = temo.file()
with open(vv,"w") as wfp:
	wfp.write(sampDAT1)

os.system("rm x1.html x2.html x3.html")

rls0 =nm.mcut(i=vv,f="a,b:e",tag="check")
rls1 =nm.mcut("i={} f=a,b,c".format(vv)).mjoin("k=a",f="e",m=rls0).run()
print(rls1)

xxx=[]
nm.mcut(i=vv,f="a,b,c").mjoin("k=a",f="e",m=rls0).writelist(xxx).run(msg="on")
print(xxx)
rls2 =nm.mcut(i=vv,f="a,b,c").mjoin("k=a",f="e",m=rls0,o=xxx).run(msg="on")
print(xxx)
rls2 =nm.mcut(i=vv,f="a,b,c").mjoin("k=a",f="e",m=rls0).run(msg="on")
print(rls2)

nm.mcut("i={} f=a,b,c".format(vv)).mjoin("k=a",f="e",m=rls0).writelist(xxx).drawModel("x1.html")

nm.mcut(i=vv,f="a,b,c").mjoin("k=a",f="e",m=rls0,o=xxx).drawModel("x2.html")


rls3 = nm.readcsv(vv)
rls4 = rls3.mcut(f="a,b:e")
rls3.mcut("f=a,b,c").mjoin("k=a",f="e",m=rls4).writecsv("vvv").drawModel("x3.html")


