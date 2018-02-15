#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import nysol.vsop._vsoplib as VSOP

from  nysol.util.mmkdir import mkDir
import nysol.util.mtemp as mtemp


lcmRUN="""#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.vsop._vsoplib as VSOP

args = sys.argv

#"FQ","indat/dat.lcm",4,"indat/dat.order",5
if len(args)==6 :
	xf = VSOP.lcm(args[1],args[2],int(args[3]),args[4],int(args[5]))
	xf.show()
elif len(args)==5 :
	xf = VSOP.lcm(args[1],args[2],int(args[3]),args[4])
	xf.show()
else:
	xf = VSOP.lcm(args[1],args[2],int(args[3]))
	xf.show()

"""

symRUN="""#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.vsop._vsoplib as VSOP

args = sys.argv

xf = VSOP.lcm(args[1],args[2],int(args[3]),args[4],int(args[5]))
xf.show()
xf.symgrp().show()
xf.permitsym(2).show()

"""

dataZDD="""_i 11
_o 3
_n 7
4 9 F T
228 10 F 5
1062 11 4 228
284 10 F 4
2 10 F T
580 11 284 2
1056 11 284 228
1062
580
1056
"""

orderZDD="""1 2 3 4 5 6 7 8 9"""

lcmZDD="""1 2 4 3 5 6 7 8 9
1 2 3 4 5 7 8 9
1 2 3 6
1 2 4 5 6 8
1 2 5 8
1 2 9
1 2 4 8 9
"""

# 必要ファイル出力
temo = mtemp.Mtemp()
scpfile =  temo.file()
zddfile =  temo.file()
ordfile =  temo.file()
lcmfile =  temo.file()
symscpfile =  temo.file()


with open(scpfile,"w") as wfp:
	wfp.write(lcmRUN)

with open(zddfile,"w") as wfp:
	wfp.write(dataZDD)

with open(ordfile,"w") as wfp:
	wfp.write(orderZDD)

with open(lcmfile,"w") as wfp:
	wfp.write(lcmZDD)

with open(symscpfile,"w") as wfp:
	wfp.write(symRUN)

import os
os.system("cat "+symscpfile )

mkDir("output",rm=True)

VSOP.symbol("a")
VSOP.symbol("b")
a=VSOP.itemset("a")
b=VSOP.itemset("b")
print(VSOP.totalsize())
# 2

xf=2*VSOP.itemset("a b")+3*VSOP.itemset("b")+4
xf.show()
#  2 a b + 3 b + 4
xf.show("map")
# a : b
#     |     0     1
#   0 |     4     3
#   1 |     0     2


print(VSOP.totalsize())
#5

VSOP.symbol("c")
VSOP.symbol("d")

c=VSOP.itemset("c")
d=VSOP.itemset("d")

xf=VSOP.itemset("a b")+2*VSOP.itemset("b c")+3*VSOP.itemset("d")+4
xf.show
#  a b + 2 b c + 3 d + 4
xf.show("map")
# a b : c d
#     |    00    01    11    10
#  00 |     4     3     0     0
#  01 |     0     0     0     2
#  11 |     1     0     0     0
#  10 |     0     0     0     0


xx=VSOP.itemset("a b")+2*VSOP.itemset("b c")+4
xx.show()
#  a b + 2 b c + 4
xx.show("map")
# a b : c d
#     |    00    01    11    10
#  00 |     4     0     0     0
#  01 |     0     0     0     2
#  11 |     1     0     0     0
#  10 |     0     0     0     0

xx.show("rmap")
# a : b c
#     |    00    01    11    10
#   0 |     4     0     2     0
#   1 |     0     0     0     1

xf=(a+1)*(b+1)*(c+1)*(d+1)
xf.show()
# a b c d + a b c + a b d + a b + a c d + a c + a d + a + b c d + b c +
#   b d + b + c d + c + d + 1 


xg=(a+b)*(b+c)*(c+d)*(d+a)
xg.show()
# 2 a b c d + 3 a b c + 3 a b d + 3 a c d + a c + 3 b c d + b d

#puts xf.inspect
# a b c d + a b c + a b d + a b + a c d + a c + a d + a + b c d + b c +
#   b d + b + c d + c + d + 1 


print(xg.density())
# 0.4375

print(xf.density())
# 1.0

#VSOP.coerce(xf)[0].show()
# a b c d + a b c + a b d + a b + a c d + a c + a d + a + b c d + b c +
#  b d + b + c d + c + d + 1


xh=(a+1)*(a+1)*(a+1)*(a+1)
xh.show
# 15 a + 1


print(VSOP.symbol())
# a b c d 

xa=VSOP.itemset("a b c")
xa.show()
# a b c


xq=VSOP.constant(4)
(xq*xa).show()
# 4 a b c

(4*xa).show()
# 4 a b c

(xa+"a b").show()
# a b c + a b


xz=VSOP.constant(0)
for item in ["a","b","c"]:
	xz += item

xz.show()
# a + b + c

xz=VSOP.constant(0)
for i in range(1,6):
	xz += "item_" + str(i)

xz.show()
#  item_1 + item_2 + item_3 + item_4 + item_5

(a*3).show()
# 3 a

(-2*a).show()
# - 2 a

((a+2*b+3*c)*4).show()
# 4 a + 8 b + 12 c

xx = a + 2*a*b+ 3*c
xx.show()
# 2 a b + a + 3 c 

(xx*'d').show()
# 2 a b d + a d + 3 c d　


(xx*'b').show()
# 3 a b + 3 b c

(4*xx*'a').show()
# 8 a b + 12 a c + 4 a


((a+b)*(c+d)).show()
# a c + a d + b c + b d

((a+b)*(b+c)).show()
# a b + a c + b c + b

((a+b)*(a+b)).show()
# 2 a b + a + b

((a+b)*(a-b)).show()
#  a - b


xx=(3*a)+(2*b)
xy=(2*a)+(2*b)+(4*c)
xx.show()
#  3 a + 2 b
xy.show()
# 2 a + 2 b + 4 c

(xx+xy).show()
# 5 a + 4 b + 4 c

(+xx).show()
# 3 a + 2 b

(xx-xy).show()
# a - 4 c

(xy-xx).show()
# - a + 4 c

(-xx).show()
# - 3 a - 2 b


xx=(13*a)+(3*b)
xx.show()
# 13 a + 3 b

(xx/5).show()
# 2 a

(xx%5).show()
# 3 a + 3 b



xx=(7*a*b)+(5*b*c)
xy=(7*a*b)+(5*b*c)+(2*c)
xx.show()
# 7 a b + 5 b c

xy.show()
# 7 a b + 5 b c + 2 c

(xx/b).show()
# 7 a + 5 c

(xy/(3*b)).show()
#  2 a + c

(xx%b).show()
# 0

(xy%(3*b)).show()
# a b + 2 b c + 2 c


xx=(2*a*b)+(4*a*c)+(a*d)-(2*b*c)+(3*b*d)
xy=a+b
xx.show()
# 2 a b + 4 a c + a d - 2 b c + 3 b d
xy.show()
# a + b

(xx/a).show()
# 2 b + 4 c + d

(xx/b).show()
# 2 a - 2 c + 3 d

#xx/a,xx/bの共通項の最小
(xx/xy).show()
# - 2 c + d

(xx%xy).show()
# 2 a b + 6 a c + 2 b d


#除算その2
xxxa = 3*a - 4*b + 4 *b  * c + 5 *a  * c  -6 *a * b
xxxb = a + b 
(xxxa/xxxb).show()
#  4 c + 3

(xxxa%xxxb).show()
#  - 6 a b + a c - 7 b

xx=(3*a)+(2*b)+(2*c)
xy=(2*a)+(2*b)+(4*c)
xx.show()
# 3 a + 2 b + 2 c

xy.show()
# 2 a + 2 b + 4 c

(xx==xy).show()
#  b

(xx>=xy).show()
# a + b

(xx!=xy).show()
# a + c

(xx<=xy).show()
# b + c
(xx<xy).show()
# c

#print(xx===xy)
#false
#print(xx===xx)
#true

print(xx.same(xy))
#false
print(xx.same(xx))
#true

print(xx.diff(xy))
#true
print(xx.diff(xx))
#false


xx=(3*a)+(2*b)+(2*c)
xy=(2*a)+(2*b)
xx.show
#  3 a + 2 b + 2 c
xy.show
# 2 a + 2 b

(xx>xy).show
# a + c

(xx!=xy).show
# a + c

(xx==xy).show 
# b

 
xa=5*VSOP.itemset("a b c")-3*VSOP.itemset("a b")+2*VSOP.itemset("b c")+1*VSOP.itemset("c")
xa.show
#  5 a b c - 3 a b + 2 b c + c

xa.show("bit")
#  3: a b
#  2: a b c + a b + b c
#  1: b c
#  0: a b c + a b + c

xa.show("case")
# 5:  a b c
# 2:  b c
# 1:  c
# -3:  a b


xc=VSOP.constant(10)
xc.show()
#  10


(xc*"a").show()
# 10 a

#print(xc.to_i*10)
# 100

xa=VSOP.constant(0)
xa+="x" 
xa+="x z" 
xa+="z"
xa.show()


xf=a+b+a*b
xf.show()
# a b + a + b


print(xf.count())


xg=a+b+a*b+1
xg.show()
#  a b + a + b + 1

print(xg.count())
# 4

xc=VSOP.constant(0)
print(xc.count())
# 0

xd=VSOP.constant(1)
print(xd.count())
# 1

xx=a*b+2*b*c+3*d+4
xx.show()
#  a b + 2 b c + 3 d + 4

xx.csvout("output/output1.csv")

xf1=(a*b*c)
xf1.show()
#  a b c

xf1.show("decomp")
# AND( a b c )

xf2=((a*b*c)+(a*b))
xf2.show()
# a b c + a b

xf2.show("decomp")
# AND( a b OR( c 1 ) )


xf3=((a*b*c)+(a*b)+(b*c))
xf3.show()
#  a b c + a b + b c
xf3.show("decomp")
# AND( [ a c ] b )


xf4=((a*b*c)+(a*b)+(b*c)+(c*a))
xf4.show()
# a b c + a b + a c + b c

xf4.show("decomp")
# [ a b c ]

xf=a+2*a*b+3*b
xf.delta(a).show()
#  3 a b + 2 b + 1

xf.delta(b).show()
#  a b + 2 a + 3

xf.delta(a*b).show()
#  3 a + b + 2

xf.delta(1).show()
#  2 a b + a + 3 b

xf=((a*b*c)+2*(a*b)+(b*c)+1)
xf.show()
#  a b c + 2 a b + b c + 1

xg=2*a*b + a
xg.show()
#  2 a b + a

xf.delta(xg).show()
#  a b c + 2 a b + 2 a c + a + b c + 2 b + 2 c + 4

xf=(a+1)*(b+1)*(c+1)
xf.show()
# a b c + a b + a c + a + b c + b + c + 1

xx=2*a+2*b+4*a*c
xx.show()
#  4 a c + 2 a + 2 b

for xxv in xx:
	xxv.show()
# 4 a c
# 2 a
# 2 b

xx=(2*a)+(2*b)+(4*a*c) 
xx.show()

#   4 a c + 2 a + 2 b
for val,xxv,t,bot in xx.itemiter():
	print(val)
	xxv.show()
	print(t)
	print(bot)

xf=5*a*b*c+3*a*b+2*b*c+c
xf.show()

xf.export()

xt=(a*b)+(a)+(b*c*d)+(a*b*c)+a
xt.show()

#  a b c + a b + 2 a + b c d

xt.freqpatA(1).show()
#  a b c + a b + a c + a + b c d + b c + b d + b + c d + c + d + 1

xt.freqpatM(1).show()
#  a b c + b c d

xt.freqpatC(1).show()
#  a b c + a b + a + b c d + b c + b + 1

xt.freqpatA(2).show()
#  a b + a + b c + b + c + 1

xt.freqpatM(2).show()
#  a b + b c

xt.freqpatC(2).show()
#  a b + a + b c + b + 1
  
xt.freqpatA(3).show()
#  a + b + 1

xt.freqpatM(3).show()
#  a + b

xt.freqpatC(3).show()
#  a + b + 1

xa=a*b+2*b*c+3*d+4
xa.show()

#  a b + 2 b c + 3 d + 4
xh=xa.hashout()

print(VSOP.partly)

for kv in xh.items():
	print(kv[0])
	print(kv[1])


xa=VSOP.itemset("a b")+11*VSOP.itemset("b c")+30*VSOP.itemset("d")+4
xa.show()
#  a b + 11 b c + 30 d + 4

xa.show("hex")
#  a b + B b c + 1E d + 4

xx = 3*a+2*b+2*c
xy = 2*a+2*b+4*c 
xx.show()
# 3 a + 2 b + 2 c
xy.show()
#  2 a + 2 b + 4 c


((xx>xy).iif(xx,xy)).show()
# 3 a + 2 b + 4 c 

(xx>xy).iif(xx,0).show()
# 3 a

(xx==xy).iif(xx,0).show()
# 2 b

xf=VSOP.zddread(zddfile)
xf.show()

xf=((a*b*c)+(2*a*b)+(b*c))
xf.show()
# a b c + 2 a b + b c
xf.items().show()
# 3 a + 4 b + 2 c

xx3=VSOP.itemset("りんご ばなな")
xx3.show()
#  りんご ばなな

VSOP.symbol("e",1.0)
VSOP.symbol("f",0.5)
VSOP.symbol("g",2.0)
e=VSOP.itemset("e")
f=VSOP.itemset("f")
g=VSOP.itemset("g")


xf = e*f+f*g+g*e
xf.show()
# e f + e g + f g

xf.maxcover().show()
# e g

print(xf.maxcost())
# 3.0


xf.mincover().show()
# e f

print(xf.mincost())
# 1.5

xx = 5*a+3*b+c
xx.show()
# 5 a + 3 b + c

xx.maxweight().show()
# 5

xx = 5*a+3*b+c+10 
xx.show()
# 5 a + 3 b + c + 10

xx.maxweight().show()
# 10

xx = 5*a+3*b+5*c+2
xx.show()
#  5 a + 3 b + 5 c + 2

xx.termsEQ(xx.maxweight()).show()
#  5 a + 5 c 

xf=a+2*a*b+3*b
xf.meet(a).show()
# 3 a + 3

xf.meet(b).show()
# 5 b + 1

xf.meet(a*b).show()
#  2 a b + a + 3 b

xf.meet(1).show()
#  6

xf=((a*b*c)+2*(a*b)+(b*c)+1)
xf.show()
#  a b c + 2 a b + b c + 1

xg=2*a*b + a
xg.show()
#  2 a b + a

xf.meet(xg).show()
#  6 a b + 3 a + 2 b + 4
 
xx = 5*a-3*b+c 
xx.show()
# 5 a - 3 b + c

xx.minweight().show()
#  - 3

xx = 5*a-3*b+c-10
xx.show()
#  5 a - 3 b + c - 10

xx.minweight().show()
# - 10

xx = 5*a-3*b+5*c-3
xx.show()
# 5 a - 3 b + 5 c - 3

xx.termsEQ(xx.minweight()).show()
#  - 3 b - 3


xx = 5*a+3*b+c
xy = a+b
xz = a*b

xx.show()
#  5 a + 3 b + c
xy.show()
#  a + b
xz.show()
#  a b

xx.permit(xy).show()
#  5 a + 3 b

xx.permit(xz).show()
#  5 a + 3 b  

xx.permit("a").show()
#  5 a

#ここまでオK
xx = 5*a+3*b+c
xy = a+b
xz = a*b
xx.show()
#  5 a + 3 b + c
xy.show()
#  a + b
xz.show()
#  a b
  
xx.restrict(xy).show()
#  5 a + 3 b
xx.restrict(xz).show()
#  0
xx.restrict("a").show()
#  5 a

xx=5*VSOP.itemset("a b c")-3*VSOP.itemset("a b")+2*VSOP.itemset("b c")+1*VSOP.itemset("c")
xx.show()

#  5 a b c - 3 a b + 2 b c + c
print(xx.size())
# 10 

VSOP.symbol("ax",1.0)
VSOP.symbol("bx",0.5)
VSOP.symbol("cx",2.0,"top")
VSOP.symbol("dx")
print(VSOP.symbol())



xx = 5*a+3*b+c
xx.show()
#  5 a + 3 b + c

xx.termsEQ(3).show()
#  3 b

xx.termsGE(3).show()
#  5 a + 3 b

xx.termsLE(3).show()
# 3 b + c

xx.termsNE(3).show()
#  5 a + c

xx.termsGT(10).show()
#  0

xx.termsLT(3).show()
# c

xx = 2*a+2*b+4*a*c
xx.show()
#  4 a c + 2 a + 2 b


#array = xx.to_a
#array.each{|v|
#	puts v
#}
# 4 a c
# 2 a
# 2 b


VSOP.constant(10).show()
# 10

#p a.to_i
# nil

xx = 2*a+2*b+4*a*c
xx.show()
#  4 a c + 2 a + 2 b
#puts xx.to_s
# 4 a c + 2 a + 2 b

xx=5*VSOP.itemset("a b c")-3*VSOP.itemset("a b")+2*VSOP.itemset("b c")+1*VSOP.itemset("c")
xx.show()
# 5 a b c - 3 a b + 2 b c + c
print(xx.size())
# 10
print(VSOP.totalsize())
# 47


xy=-3*VSOP.itemset("a c")
xy.show()
# - 3 a c
print(xy.size())
# 5
print(VSOP.totalsize())
# 51

xx = 5*a+3*b+c
xx.show()
#  5 a + 3 b + c

print(xx.totalweight())
#  9

xx = 5*a+3*b+c- 10
xx.show()

print(xx.totalweight())

# -1

VSOP.symbol("a2",1.0)
VSOP.symbol("b2",0.5)
VSOP.symbol("c2",2.0)

a2=VSOP.itemset("a2")
b2=VSOP.itemset("b2")
c2=VSOP.itemset("c2")

print(a2.cost())
# 1.0

xf = a2 
xf*=b2
xf.show()
# a2 b2

print(xf.cost())
xf+=2*a2 + c2 + 3
xf.show()

#   a2 b2 + 2 a2 + c2 + 3

print(xf.cost())
#  7.5


# LCMはここまでに登録された変数順が関係するのでややこしい
# 別々に動かす
import os
os.system("python "+ scpfile + " F " + lcmfile + " 4")
os.system("python "+ scpfile + " M " + lcmfile + " 4")
os.system("python "+ scpfile + " C " + lcmfile + " 4")
# x6 x3 x2 x1 + x6 x3 x2 + x6 x3 x1 + x6 x3 + x6 x2 x1 + x6 x2 + x6 x1 +
#  x6 + x5 x3 x2 x1 + x5 x3 x2 + x5 x3 x1 + x5 x3 + x5 x2 x1 + x5 x2 + x5
#  x1 + x5 + x4 x2 x1 + x4 x2 + x4 x1 + x4 + x3 x2 x1 + x3 x2 + x3 x1 +
#  x3 + x2 x1 + x2 + x1 + 1
# x6 x3 x2 x1 + x5 x3 x2 x1 + x4 x2 x1
# x6 x3 x2 x1 + x5 x3 x2 x1 + x4 x2 x1 + x3 x2 x1 + x2 x1

os.system("python "+ scpfile + " FQ " + lcmfile + " 4")
os.system("python "+ scpfile + " MQ " + lcmfile + " 4")
os.system("python "+ scpfile + " CQ " + lcmfile + " 4")
# 4 x6 x3 x2 x1 + 4 x6 x3 x2 + 4 x6 x3 x1 + 4 x6 x3 + 4 x6 x2 x1 + 4 x6
#  x2 + 4 x6 x1 + 4 x6 + 4 x5 x3 x2 x1 + 4 x5 x3 x2 + 4 x5 x3 x1 + 4 x5
#  x3 + 4 x5 x2 x1 + 4 x5 x2 + 4 x5 x1 + 4 x5 + 4 x4 x2 x1 + 4 x4 x2 + 4
#  x4 x1 + 4 x4 + 5 x3 x2 x1 + 5 x3 x2 + 5 x3 x1 + 5 x3 + 7 x2 x1 + 7 x2
#  + 7 x1 + 7
# 4 x6 x3 x2 x1 + 4 x5 x3 x2 x1 + 4 x4 x2 x1
# 4 x6 x3 x2 x1 + 4 x5 x3 x2 x1 + 4 x4 x2 x1 + 5 x3 x2 x1 + 7 x2 x1

os.system("python "+ scpfile + " F " + lcmfile + " 4 " + ordfile)
os.system("python "+ scpfile + " M " + lcmfile + " 4 " + ordfile)
os.system("python "+ scpfile + " C " + lcmfile + " 4 " + ordfile)
# x9 x2 x1 + x9 x2 + x9 x1 + x9 + x8 x5 x2 x1 + x8 x5 x2 + x8 x5 x1 + x8
#  x5 + x8 x4 x2 x1 + x8 x4 x2 + x8 x4 x1 + x8 x4 + x8 x2 x1 + x8 x2 + x8
#  x1 + x8 + x5 x2 x1 + x5 x2 + x5 x1 + x5 + x4 x2 x1 + x4 x2 + x4 x1 +
#  x4 + x2 x1 + x2 + x1 + 1
# x9 x2 x1 + x8 x5 x2 x1 + x8 x4 x2 x1
# x9 x2 x1 + x8 x5 x2 x1 + x8 x4 x2 x1 + x8 x2 x1 + x2 x1

os.system("python "+ scpfile + " FQ " + lcmfile + " 4 " + ordfile)
os.system("python "+ scpfile + " MQ " + lcmfile + " 4 " + ordfile)
os.system("python "+ scpfile + " CQ " + lcmfile + " 4 " + ordfile)
# 4 x9 x2 x1 + 4 x9 x2 + 4 x9 x1 + 4 x9 + 4 x8 x5 x2 x1 + 4 x8 x5 x2 + 4
#  x8 x5 x1 + 4 x8 x5 + 4 x8 x4 x2 x1 + 4 x8 x4 x2 + 4 x8 x4 x1 + 4 x8 x4
#  + 5 x8 x2 x1 + 5 x8 x2 + 5 x8 x1 + 5 x8 + 4 x5 x2 x1 + 4 x5 x2 + 4 x5
#  x1 + 4 x5 + 4 x4 x2 x1 + 4 x4 x2 + 4 x4 x1 + 4 x4 + 7 x2 x1 + 7 x2 +
#  7 x1 + 7
# 4 x9 x2 x1 + 4 x8 x5 x2 x1 + 4 x8 x4 x2 x1
# 4 x9 x2 x1 + 4 x8 x5 x2 x1 + 4 x8 x4 x2 x1 + 5 x8 x2 x1 + 7 x2 x1


os.system("python "+ scpfile + " F " + lcmfile + " 4 \"\" 3" )
os.system("python "+ scpfile + " M " + lcmfile + " 4 \"\" 3" )
os.system("python "+ scpfile + " C " + lcmfile + " 4 \"\" 3" )
# x5 x8 x2 + x5 x8 x1 + x5 x8 + x5 x2 x1 + x5 x2 + x5 x1 + x5 + x4 x8 x2
#  + x4 x8 x1 + x4 x8 + x4 x2 x1 + x4 x2 + x4 x1 + x4 + x9 x2 x1 + x9 x2
#  + x9 x1 + x9 + x8 x2 x1 + x8 x2 + x8 x1 + x8 + x2 x1 + x2 + x1 + 1
# x9 x2 x1
# x9 x2 x1 + x8 x2 x1 + x2 x1


os.system("python "+ scpfile + " FQ " + lcmfile + " 4 \"\" 3" )
os.system("python "+ scpfile + " MQ " + lcmfile + " 4 \"\" 3" )
os.system("python "+ scpfile + " CQ " + lcmfile + " 4 \"\" 3" )
# 4 x5 x8 x2 + 4 x5 x8 x1 + 4 x5 x8 + 4 x5 x2 x1 + 4 x5 x2 + 4 x5 x1 + 4
#  x5 + 4 x4 x8 x2 + 4 x4 x8 x1 + 4 x4 x8 + 4 x4 x2 x1 + 4 x4 x2 + 4 x4
#  x1 + 4 x4 + 4 x9 x2 x1 + 4 x9 x2 + 4 x9 x1 + 4 x9 + 5 x8 x2 x1 + 5 x8
#  x2 + 5 x8 x1 + 5 x8 + 7 x2 x1 + 7 x2 + 7 x1 + 7
# 4 x9 x2 x1
# 4 x9 x2 x1 + 5 x8 x2 x1 + 7 x2 x1

os.system("python "+ scpfile + " F " + lcmfile + " 4 " + ordfile + " 3")
os.system("python "+ scpfile + " M " + lcmfile + " 4 " + ordfile + " 3")
os.system("python "+ scpfile + " C " + lcmfile + " 4 " + ordfile + " 3")
# x9 x2 x1 + x9 x2 + x9 x1 + x9 + x8 x5 x2 + x8 x5 x1 + x8 x5 + x8 x4 x2
#  + x8 x4 x1 + x8 x4 + x8 x2 x1 + x8 x2 + x8 x1 + x8 + x5 x2 x1 + x5 x2
#  + x5 x1 + x5 + x4 x2 x1 + x4 x2 + x4 x1 + x4 + x2 x1 + x2 + x1 + 1
# x9 x2 x1
# x9 x2 x1 + x8 x2 x1 + x2 x1

os.system("python "+ scpfile + " FQ " + lcmfile + " 4 " + ordfile + " 3")
os.system("python "+ scpfile + " MQ " + lcmfile + " 4 " + ordfile + " 3")
os.system("python "+ scpfile + " CQ " + lcmfile + " 4 " + ordfile + " 3")
# 4 x9 x2 x1 + 4 x9 x2 + 4 x9 x1 + 4 x9 + 4 x8 x5 x2 + 4 x8 x5 x1 + 4 x8
#  x5 + 4 x8 x4 x2 + 4 x8 x4 x1 + 4 x8 x4 + 5 x8 x2 x1 + 5 x8 x2 + 5 x8
#  x1 + 5 x8 + 4 x5 x2 x1 + 4 x5 x2 + 4 x5 x1 + 4 x5 + 4 x4 x2 x1 + 4 x4
#  x2 + 4 x4 x1 + 4 x4 + 7 x2 x1 + 7 x2 + 7 x1 + 7
# 4 x9 x2 x1
# 4 x9 x2 x1 + 5 x8 x2 x1 + 7 x2 x1

os.system("python "+ symscpfile + " FQ " + lcmfile + " 4 " + ordfile + " 5")
# 4 x9 x2 x1 + 4 x9 x2 + 4 x9 x1 + 4 x9 + 4 x8 x5 x2 x1 + 4 x8 x5 x2 + 4
#  x8 x5 x1 + 4 x8 x5 + 4 x8 x4 x2 x1 + 4 x8 x4 x2 + 4 x8 x4 x1 + 4 x8 x4
#  + 5 x8 x2 x1 + 5 x8 x2 + 5 x8 x1 + 5 x8 + 4 x5 x2 x1 + 4 x5 x2 + 4 x5
#  x1 + 4 x5 + 4 x4 x2 x1 + 4 x4 x2 + 4 x4 x1 + 4 x4 + 7 x2 x1 + 7 x2 +
#  7 x1 + 7
# x9 x8 x5 x4 x2 x1
# 4 x9 x2 + 4 x9 x1 + 4 x9 + 4 x8 x5 + 4 x8 x4 + 5 x8 x2 + 5 x8 x1 + 5 x8
#  + 4 x5 x2 + 4 x5 x1 + 4 x5 + 4 x4 x2 + 4 x4 x1 + 4 x4 + 7 x2 x1 + 7 x2
#  + 7 x1 + 7









