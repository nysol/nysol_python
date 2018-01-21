#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import nysol.util.margs as margs 
import nysol.mod as nm

inf = "xxx.csv"
id = ["id"]
fld = ["F_x"]
sflds = ["time"]


cflds= set([])

for x in id:
	cflds.add(x)

for x in fld:
	cflds.add(x)

for x in sflds:
	cflds.add(x)


cfld = list(cflds)

base = nm.mcut(f=cfld,i=inf)


#abs_energy,absolute_sum_of_changes,abs_energy
f1 =   nm.mcal(c="${F_x}*${F_x}",a="abs_energy", i=base)
f1 <<= nm.mcal(c="abs(${F_x}-#{F_x})",a="absolute_sum_of_changes")
f1 <<= nm.msum(k="id",f="absolute_sum_of_changes,abs_energy",o="rls1.csv")
f1.run()

#平均と分散
base.msummary(k="id",f="F_x",c="mean,var,count,sd",o="sumary.csv").run()

#autocorrelation agg ここはもっといい方法ありそう
f1 =   nm.mcut(f="F_x",i=base,r=True)
f1 <<= nm.mjoin(k="id",m="sumary.csv",f="mean,var,count") 
f1 <<= nm.mcombi(k="id", f="time",a="t1,t2",n=2)
f1 <<= nm.mfsort(f="t1,t2",n=True)
f1 <<= nm.mcal(c="${t2}-${t1}",a="rag")
f1 <<= nm.mjoin(k="id,t1", m=inf, K="id,time",f="F_x:F_x_t1")
f1 <<= nm.mjoin(k="id,t2", m=inf, K="id,time",f="F_x:F_x_t2")
f1 <<= nm.mcal(c='(${F_x_t1}-${mean})*(${F_x_t2}-${mean})',a="sub_t1t2") 
f1 <<= nm.msum(k="id,rag",f="sub_t1t2") 
f1 <<= nm.mcal(c="${sub_t1t2}/(${count}-${rag})/${var}",a="t1t2sum")
f1 <<= nm.msummary(k="id",c="mean,median,var",f="t1t2sum",o="rls2.csv")
f1.run()




#linregress chunk(5,10,50)
f1 =   nm.mnumber(k="id",q=True,s="time%n",a="cnt",i=base)
f1 <<= nm.mcal(c='int(${cnt}/5),int(${cnt}/10),int(${cnt}/50)',a="cs5,cs10,cs50")
funcList = []
for lg in [5,10,50]:
	f2_0 = None
	f2_0 <<= nm.msummary(k="id,cs%s"%(lg),i=f1,f="F_x",c="max,min,mean,median")
	
	f2_1 = None
	f2_1 <<= nm.mcount(k="id",i=f2_0,a="cnt")
	f2_1 <<= nm.mcal(c='${cnt}-2',a="df")
		
	f2_2 = None	
	f2_2 <<= nm.mnumber(k="id",q=True,a="x",i=f2_0)

	f2_3 = None	
	f2_3 <<= nm.msummary(i=f2_2,k="id",c="var,mean",f="x,max,min,mean,median",dlog="chk1")
	f2_3 <<= nm.msortf(f="id,fld")

	f2_4 = None
	f2_4 <<= nm.msim(i=f2_2,k="id",f="x,max,min,mean,median",c="covar")
	f2_4 <<= nm.mselstr(f="fld1,fld2",v="x")
	f2_4 <<= nm.mjoin(k="id,fld1",K="id,fld",f="var:var1,mean:mean1",m=f2_3)
	f2_4 <<= nm.mjoin(k="id,fld2",K="id,fld",f="var:var2,mean:mean2",m=f2_3)
	f2_4 <<= nm.mjoin(k="id",f="df",m=f2_1,dlog="chk")
	f2_4 <<= nm.mcal(c="sqrt(${var1}*${var2})",a="mm")
	f2_4 <<= nm.mcal(c="if(${mm}==0,0,${covar}/${mm})",a="rvalue")
	f2_4 <<= nm.mcal(c="${rvalue}*sqrt(${df}/((1-${rvalue})*(1+${rvalue})))",a="t")
	f2_4 <<= nm.mcal(c="${covar}/${var1}",a="slope")
	f2_4 <<= nm.mcal(c="${mean2}-${slope}/${mean1}",a="intercept")
	f2_4 <<= nm.mcal(c="sqrt((1-${rvalue}^2)*${var2}/${var1}/${df})",a="stder",o="rls3_%d.csv"%(lg))

	#まとめて動かそうとするとエラーになるとき有り(runsおかしい?)
	#funcList.append(f2_4)
	f2_4.run()

#nm.runs(funcList,msg="on")


#approximate_entropy
mval =2
rvals =[0.1,0.3,0.5,0.7,0.9]

# m
fpara1_0 = ",".join([ "F_x%d:F_x%s_t"%(i,i) for i in range(mval)])
fpara1_1c = "max(%s)"%(",".join(["abs(${F_x%d}-${F_x%s_t})"%(i,i) for i in range(mval)]))

f3_1 =   nm.mslide(k="id",s="time%n",f="F_x",t=mval-1,i=base)
f3_1 <<= nm.mfldname(f="F_x:F_x0")
f3_1 <<= nm.mjoin(k="id",f="sd,count",m="sumary.csv")

f3_2 =   nm.mnjoin(k="id",i=f3_1,m=f3_1,f=fpara1_0)
f3_2 <<= nm.mcal(c=fpara1_1c,a="F_x_max")

# m+1

pfpara1_0 = ",".join([ "F_x%d:F_x%s_t"%(i,i) for i in range(mval+1)])
pfpara1_1c = "max(%s)"%(",".join([ "abs(${F_x%d}-${F_x%s_t})"%(i,i) for i in range(mval+1)]))

pf3_1 =   nm.mslide(k="id",s="time%n",f="F_x",t=mval,i=base)
pf3_1 <<=   nm.mfldname(f="F_x:F_x0")
pf3_1 <<= nm.mjoin(k="id",f="sd,count",m="sumary.csv")

pf3_2 =   nm.mnjoin(k="id",i=pf3_1,m=pf3_1,f=pfpara1_0)
pf3_2 <<= nm.mcal(c=pfpara1_1c,a="F_x_max")

funcList2 = []

for rval in rvals:
	f3_3 =   nm.msetstr(a="r",v=rval,i=f3_2) 
	f3_3 <<= nm.mcal(c="${F_x_max}<=${r}*${sd}",a="match") 
	#ここら辺 msimでもOK?
	f3_3 <<= nm.msum(k="id,time",f="match",dlog="chk0") 
	f3_3 <<= nm.mcal(c="${match}/(${count}-%d+1)"%(mval),a="C") 
	f3_3 <<= nm.mcal(c="ln(${C})",a="Clog") 
	f3_3 <<= nm.msum(k="id",f="Clog") 
	f3_3 <<= nm.mcal(c="${Clog}/(${count}-%d+1)"%(mval),a="phi0") 

	f3_4 =   nm.msetstr(a="r",v=rval,i=pf3_2) 
	f3_4 <<= nm.mcal(c="${F_x_max}<=(${r}*${sd})",a="match") 
	f3_4 <<= nm.msum(k="id,time",f="match",dlog="chk1") 
	f3_4 <<= nm.mcal(c="${match}/(${count}-%d+1)"%(mval+1),a="C") 
	f3_4 <<= nm.mcal(c="ln(${C})",a="pClog") 
	f3_4 <<= nm.msum(k="id",f="pClog") 
	f3_4 <<= nm.mcal(c="${pClog}/(${count}-%d+1)"%(mval+1),a="phi1") 

	f3_5 =   nm.mjoin(i=f3_3,m=f3_4,k="id",f="phi1")
	f3_5 <<= nm.mcal(c="abs(${phi0}-${phi1})",a="approximate_entropy")
	f3_5 <<= nm.mcut(f="id,r,approximate_entropy",o="rls4_%g.csv"%(rval))
	
	funcList2.append(f3_5)
	#f3_5.run()

print "zzz1"
#f3_1 =   nm.mslide(k="id",s="time%n",f="F_x",t=3,i=base)
#ほんとにいけてる？
nm.runs(funcList2)
print "zzz2"



#autocorrelation lag
#autocorrelation agg ここはもっといい方法ありそう

f5 = nm.mjoin(k="id",i=base,m="sumary.csv",f="mean,var,count") 
lags=[1,2,3,4,5,6,7,8,9]
for lag in lags:
	f5_1 = nm.mslide(k="id",s="time%n", i=f5,f="F_x:F_x_L",t=lag,l=True)
	f5_1 <<= nm.mcal(c='(${F_x}-${mean})*(${F_x_L}-${mean})',a="F_x_m") 
	f5_1 <<= nm.msum(k="id",f="F_x_m") 
	f5_1 <<= nm.mcal(c="${F_x_m}/(${count}-%d)/${var}"%(lag),a="autocorrelation_lag",o="rls5_%d.csv"%(lag))
	f5_1.run()



#binned_entropy

maxbins=[10]
for maxbin in maxbins:
	f6 =   nm.mbucket(k=id,f="F_x:F_x_no",n=maxbin,i=base,rng=True) 
	f6 <<= nm.mcount(k="id,F_x_no",a="hcount")
	f6 <<= nm.mjoin(k=id,m="sumary.csv",f="count")
	f6 <<= nm.mcal(c='(${hcount}/${count})*ln(${hcount}/${count})',a="probs")
	f6 <<= nm.msum(k="id",f="probs")
	f6 <<= nm.mcal(c='${probs}*-1',a="binned_entropy",o="rls6_%d.csv"%(maxbin))
	f6.run()


lags=[1,2,3]
#c3
for lag in lags:
	f7 =   nm.mslide(k="id",s="time%n", i=base,f="F_x:F_x_L",t=lag,n=True,l=True)
	f7 <<= nm.mslide(k="id",s="time%n",f="F_x:F_x_L2",t=lag*2,l=True)
	f7 <<= nm.mcal(c="${F_x}*${F_x_L}*${F_x_L2}",a="rls")
	f7 <<= nm.msummary(k="id",c="mean:c3",f="rls",o="rls7_%d.csv"%(lag))
	f7.run()

#change_quantiles
quantile=[ 
[0.0,0.2],[0.0,0.4],[0.0,0.6],[0.0,0.8],[0.0,1.0],
[0.2,0.4],[0.2,0.6],[0.2,0.8],[0.2,1.0],[0.8,1.0],
[0.4,0.6],[0.4,0.8],[0.4,1.0],[0.6,0.8],[0.6,1.0]
]
absbool =[True,False]
funclist9=[]
for ql,qh in quantile:
	for abs_b in absbool:
		f8_0 =   nm.mjoin(i=base,k="id",f="count",m="sumary.csv")
		f8_0 <<= nm.mnumber(k="id",s="F_x%n", a="qt",e="skip")
		f8_0 <<= nm.mcal(c="${count}*%g<=${qt}&&${count}*%g>${qt}"%(ql,qh),a="qtbool",dlog="chkz")
		f8_0 <<= nm.mslide(k="id",s="time%n",r=True,f="qtbool:qtbool1")
		f8_0 <<= nm.mcal(c="${qtbool}*${qtbool1}",a="ind",dlog="chk")

		f8_1 = nm.mslide(k="id",s="time%n",r=True,i=base,f="F_x:F_xx")
		if abs_b:
			f8_1 <<= nm.mcal(c="abs(${F_x}-${F_xx})",a="diff")
		else:
			f8_1 <<= nm.mcal(c="${F_x}-${F_xx}",a="diff")
				
		f8_1 <<= nm.mjoin(k="id,time",f="ind" ,m=f8_0 )
		f8_1 <<= nm.mtonull(f="ind" ,v=0)
		f8_1 <<= nm.mcal(c="${diff}*${ind}" ,a="dval")
		f8_1 <<= nm.msummary(k="id",f="dval",c="mean,var")
		f8_1 <<= nm.mnullto(f="mean,var",v=0,o="rls8_%s_%g_%g.csv"%(abs_b,qh,ql) )
		f8_1.run()
	#funclist9.append(f8_1)

#print "zzz3"
#print funclist9
#nm.runs(funclist9,msg="on")	
#print "zzz4"

"""	
tsfresh.feature_extraction.feature_calculators 


	f2 <<= nm.msummary(i=f1,k="id",c="var,count",f="x,max,min,mean,median")

	
f1 <<= nm.msummary(k="id,cs5",f="F_x",c="max,min,mean,median")
f1 <<= nm.mnumber(k="id",q=True,a="x")

f2 <<= nm.mcross(k="id",s="fld",f="count,var")
f2 <<= nm.mselstr(f="fld",v="var",o="rlsvar",u="rlscount")
f2.run()

nm.msim(i=f1,k="id",f="x,max",c="covar",o="rlscovar").run()

f3 =   nm.mcal(c="(${x}*${max})",i="rlsvar",a="mm")
f3 <<= nm.mjoin(k="id",f="covar",m="rlscovar")

f3 <<= nm.mcal(c="if(${mm}==0,0,${covar}/${mm})",a="mmm")
f3 <<= nm.mjoin(k="id",f="x:cnt",m="rlscount")
#mmm:=> r
f3 <<= nm.mcal(c="(${cnt}-2)/((1-${mmm})*(1+${mmm}))",a="t")
f3 <<= nm.mjoin(k="id",f="x:var",m="rlsvar")
f3 <<= nm.mcal(c="${covar}/${var}",a="slope",o="rls3.csv")


f3.run()
"""
#distributions.t.sf(np.abs(t), df)

#"F_x__agg_linear_trend__f_agg_""max""__chunk_len_5__attr_""intercept"""
#"F_x__agg_linear_trend__f_agg_""max""__chunk_len_5__attr_""rvalue"""
#"F_x__agg_linear_trend__f_agg_""max""__chunk_len_5__attr_""slope"""
#"F_x__agg_linear_trend__f_agg_""max""__chunk_len_5__attr_""stderr"""
