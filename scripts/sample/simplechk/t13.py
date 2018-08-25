#!/usr/bin/python
import nysol.mcmd as nm
import nysol.util.mtemp as mtemp

sampDAT1="""\
node1,node2,simType,sim,dir,color
b,a,jaccard,0.3333333333,F,8888FF
j,c,jaccard,0.3333333333,F,8888FF
b,d,jaccard,0.3333333333,F,8888FF
a,e,jaccard,0.5,W,0000FF
d,e,jaccard,0.5,W,0000FF
"""

temo = mtemp.Mtemp()
nodes = temo.file()
with open(nodes,"w") as wfp:
	wfp.write(sampDAT1)

fo = nm.mselstr(i=nodes,f="dir",v="W")
fo1 =  nm.mcommon(k="node1,node2",K="node1,node2",r=True,m=fo.redirect("u"),i=fo)
fo1 <<= nm.mcommon(k="node1,node2",K="node2,node1",r=True,m=fo)
print(fo1.run())


fu = fo.redirect("u") 
fo2 = nm.mcommon(k="node1,node2",K="node1,node2",r=True,m=fu,i=fo)
fo2 <<= nm.mcommon(k="node1,node2",K="node2,node1",r=True,m=fo)
print(fo2.run(msg="on"))


fo0 = nm.mselstr(i=nodes,f="dir",v="W")
fx1 = fo0
fx1 <<=nm.mcommon(k="node1,node2",K="node1,node2",r=True,m=fo0.redirect("u"))
fx1 <<= nm.mcommon(k="node1,node2",K="node2,node1",r=True,m=fo0.redirect("o"))
print(fx1.run())

xfo = nm.mselstr(i=nodes,f="dir",v="W")
xfo1 = xfo.redirect("u")
xfo1 <<= nm.mcommon(k="node1,node2",K="node1,node2",r=True,i=xfo).iredirect("m")
xfo1 <<= nm.mcommon(k="node1,node2",K="node2,node1",r=True,m=xfo)
print(xfo1.run())

