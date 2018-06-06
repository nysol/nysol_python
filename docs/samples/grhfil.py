
from nysol.take.extcore import grhfil
from nysol.take.sample import Data

name="graph01"
dat=Data(name)
#dat.writeEdges(name)
dat.writeAdjMatrix(name)
Data.head(name,top=0)
grhfil(type="D+",i=name,o="xxa")
Data.head("xxa",top=0)
exit()

grhfil(type="ue_",i=name,o="xxa")
grhfil(type="UE_",i="xxa",o="rsl_UeE")
Data.head("rsl_UeE",top=0)
exit()

name="graph00"
dat=Data(name)
dat.writeAdjMatrix(name)

Data.head(name,top=0)
grhfil(type="U_",i=name,o="rsl_U")
Data.head("rsl_U",top=0)

#grhfil(type="ueE_",i=fName,separator=",",o="xxa")
#smp.head("xxa",top=0)
#grhfil(type="Ue0%",i="edgeNum1.csv",o="xxb")

