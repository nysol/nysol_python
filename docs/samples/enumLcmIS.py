import os
from nysol.take.traDB import TraDB
from nysol.take.enumLcmIs import LcmIs
from nysol.take.sample import Samples

smp=Samples()
iFile=smp.traDB1()
print("iFile:",iFile)
with open(iFile) as csv:
	dat=csv.read()
	print(dat[0:52])

db=TraDB(iFile=iFile,idFN="id",itemFN="item")
print(db)
exit()

lcm=LcmIs(db)

eArgs={}
eArgs["type"]="F"
eArgs["minCnt"]=3
eArgs["minLen"]=2
lcm.enumerate(eArgs)
oPath="./xxrsl1"
os.system("mkdir %s"%oPath)
lcm.output(oPath)
print(lcm)

eArgs={}
eArgs["type"]="C"
eArgs["minCnt"]=3
eArgs["minLen"]=2
lcm.enumerate(eArgs)
oPath="./xxrsl2"
os.system("mkdir %s"%oPath)
lcm.output(oPath)
print(lcm)


