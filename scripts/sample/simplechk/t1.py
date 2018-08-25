#!/usr/bin/env python
# -*- coding: utf-8 -*-
import nysol.mcmd as nm
import nysol.util.mtemp as mtemp
import os

sampDAT1="""\
@RELATION       testdata

@ATTRIBUTE      顧客ID  string
@ATTRIBUTE      日付    date yyyyMMdd
@ATTRIBUTE      日付時間        date yyyyMMddHHmmss
@ATTRIBUTE      単価    numeric
@ATTRIBUTE      数量    numeric
@ATTRIBUTE      商品ID  {1000,1001,1002,1005,1006,1007}

@DATA
C,20080822, '2008082,2031411',100,3,1000
A,20080809,20080809031412,101,1,1001
B,20080801,20080801031413,98,2,1002
C,20080819,20080819031414,98,1,1002
C,20080819,20080819031415,98,1,1002
C,20080819,20080819031416,100,1,1002
C,20080819,20080819031417,100,1,1002
C,20080819,20080819031418,102,1,1002
B,20080802,20080802031421,98,8,1005
B,20080802,20080802031431,100,8,1005
A,20080812,20080812031441,135,2,1005
B,20080808,20080808031451,139,1,1006
B,20080808,20080808031401,139,1,1007
"""
temo = mtemp.Mtemp()
sampleData = temo.file()
with open(sampleData,"w") as wfp:
	wfp.write(sampDAT1)

outData = temo.file()

nm.marff2csv("i={} o={}".format(sampleData,outData)).run(msg="on")
os.system("cat "+outData)
rls0 = nm.marff2csv("i={}".format(sampleData)).run(msg="on")

xxx=[["a","b","c"],["1","2","3"],["4","5","6"]]
rls1 = nm.mcut(f="a",i=xxx,tag="vvvvvv").run(msg="on")
rls2 = nm.readlist(xxx).mcut(f="a").run()
rls3=[]
nm.mcut(f="a",i=xxx).writelist(rls3).run()

print(rls1)
print(rls2)
print(rls3)

