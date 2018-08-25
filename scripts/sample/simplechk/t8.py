#!/usr/bin/python
import nysol.mcmd as nm
import os
data1 =[["a","b","c"],["1","2","3"],["4","5","6"]]

xxx=[]
n1 = nm.mselstr(i=data1,f="a",v="1")
n3 = n1.msetstr(a="d1",v="x0",o=xxx)
n2 = n1.redirect("u").msetstr(a="d2",v="x1").mproduct(m=n3,f="d1")

print(xxx)




#import nysol.mod as nm
#data1 =[["a","b","c"],["1","2","3"],["4","5","6"]]
n1 = None
n1 <<= nm.mselstr(i=data1,f="a",v="1",o="t1",u="t2")
n1 <<= nm.msetstr(a="add",v="vvv")
print(n1.run())


n2 = None
n2 <<= nm.mcut(i=data1,f="a,b,c")
n2 <<= nm.msum(f="c",o="t4")
n2 <<= nm.mcut(f="b,c",o="t5")
print(n2.run(msg="on"))

xxx1 = []
xxx2 = []
n1 = None
n1 <<= nm.mselstr(i=data1,f="a",v="1",o=xxx1,u=xxx2)
n1 <<= nm.msetstr(a="add",v="vvv")
n1.run()
print(xxx1)
print(xxx2)

n1 = None
n1 <<= nm.mselstr(i=data1,f="a",v="1",r=True).writelist(xxx1)
print(nm.runs([n1],msg="on"))

print(xxx1)
print(xxx2)
