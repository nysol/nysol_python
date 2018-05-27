
import os
from nysol.take.extcore import lcm
from nysol.take.sample import Samples

smp=Samples()

name="tra01"
dat=Samples(name)
dat.writeTra(name)
exit()

iFile=smp.numTraDB1()
smp.head(iFile,top=0)

lcm(type="F_",sup=3,i=iFile,o="xxham")
smp.head("xxham",top=0)
exit()

lcm(type="Ff_",sup=3,i=iFile,o="rsl_Ff")
smp.head("rsl_Ff",top=0)

lcm(type="Cf_",sup=3,i=iFile,o="rsl_Cf")
smp.head("rsl_Cf",top=0)

lcm(type="Mf_",sup=3,i=iFile,o="rsl_Mf")
smp.head("rsl_Mf",top=0)

lcm(type="Ff_",sup=3,l=3,i=iFile,o="rsl_Ff_l3")
smp.head("rsl_Ff_l3",top=0)

lcm(type="FfI_",sup=3,l=3,i=iFile,o="rsl_FfI_l3")
smp.head("rsl_FfI_l3",top=0)

lcm(type="FftI_",sup=3,l=3,i=iFile,o="rsl_FftI_l3")
smp.head("rsl_FftI_l3",top=0)

lcm(type="Ff",K=5,sup=10,i=iFile,so="rsl_Ff_K")
smp.head("rsl_Ff_K",top=0)

wFile=smp.numTraDB1w()
smp.head(wFile,top=0)
lcm(type="FfA_",sup=1,w=wFile,i=iFile,o="rsl_FfA_w")
smp.head("rsl_FfA_w",top=0)

