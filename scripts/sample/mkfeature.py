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

#平均と分散
base.msummary(k="id",f="F_x",c="mean,var,count,sd,ukurt,max,min,median,uvar,uskew,sum",o="sumary.csv").run()


#abs_energy,absolute_sum_of_changes,abs_energy
f1 =   nm.mcal(c="${F_x}*${F_x}",a="abs_energy", i=base)
f1 <<= nm.msortf(f="id,time%n")
f1 <<= nm.mcal(c="abs(${F_x}-#{F_x})",a="absolute_sum_of_changes")
f1 <<= nm.msum(k="id",f="absolute_sum_of_changes,abs_energy",o="rls1.csv")
f1.run()


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
	funcList.append(f2_4)
	f2_4.run()

nm.runs(funcList)


#approximate_entropy
mval =2
#rvals =[0.1,0.3,0.5,0.7,0.9]
rvals =[0.1]

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
	#f3_5.run() # << =たまにこける


print "zzz1"

#f3_1 =   nm.mslide(k="id",s="time%n",f="F_x",t=3,i=base)
#ほんとにいけてる？＝＞こけてるばあいあり
nm.runs(funcList2)
print "zzz2"

#ar_coefficient
#augmented_dickey_fuller
#あと

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

print "zzz3"
print funclist9
#nm.runs(funclist9,msg="on")	<=これだめ
print "zzz4"


#cid_ce normalise (normalizeのsdチェック)
"""
f9 = nm.mnormalize(k="id",c="z",i=base,f="F_x:F_xn")
f9 <<= nm.mslide(k="id",s="time%n",f="F_x:F_xx,F_xn:F_xxn")
f9 <<= nm.mcal(c="(${F_xx}-${F_x})^2,(${F_xxn}-${F_xn})^2" , a="diff,diffn")
f9 <<= nm.msum(k=id,f="diff,diffn")
f9 <<= nm.mcal(c="sqrt(${diff}),sqrt(${diffn})",a="cid_ce,cid_ce_n",o="rls9.csv")
f9.run()
"""
f9 = nm.mjoin(k="id",i=base,m="sumary.csv",f="mean,sd")
f9 <<= nm.mcal(c="(${F_x}-${mean})/${sd}",a="F_xn")
f9 <<= nm.mslide(k="id",s="time%n",f="F_x:F_xx,F_xn:F_xxn")
f9 <<= nm.mcal(c="(${F_xx}-${F_x})^2,(${F_xxn}-${F_xn})^2" , a="diff,diffn")
f9 <<= nm.msum(k=id,f="diff,diffn")
f9 <<= nm.mcal(c="sqrt(${diff}),sqrt(${diffn})",a="cid_ce,cid_ce_n",o="rls9.csv")
f9.run()


#count_above_mean,count_below_mean
f10 =   nm.mjoin(k="id",m="sumary.csv",f="mean",i=base)
f10 <<= nm.mcal(c="${F_x}>${mean},${F_x}<${mean}",a="above,below")
f10 <<= nm.msum(k="id",f="above,below",o="rls10.csv")
f10.run()


#Continuous WaveletTransform
#あと

#energy_ratio_by_chunks
num_segments=[10]
segment_focus=[0,1,2,3,4,5,6,7,8,9]

#rls1.csv　で二乗和計算済み
#sumary.csvでcount済み
for ns in num_segments:
	for sf in segment_focus:
		f11 = nm.mjoin(k="id",i=base,m="sumary.csv",f="count,sd")
		f11 <<= nm.mjoin(k="id",m="rls1.csv",f="abs_energy:sqsum")
		f11 <<= nm.mcal(c="int(${count}/%d)"%ns,a="sl")
		f11 <<= nm.mcal(c="%d*${sl},min(%d*${sl},${count})"%(sf,sf+1),a="st,ed")
		f11 <<= nm.msel(c="${time}>=${st}&&${time}<${ed}")
		f11 <<= nm.mcal(c="${F_x}^2",a="d2")
		f11 <<= nm.msum(k="id",f="d2")
		f11 <<= nm.mcal(c="${d2}/${sqsum}",a="energy_ratio_by_chunks",o="rls11_%d_%d.csv"%(ns,sf))
		f11.run()



#fft_aggregated 
#fft_coefficient__coeff_0
#フーリエ変換必要　あと


#first_location_of_maximum
#first_location_of_minimum
#last_location_of_maximum
#last_location_of_minimum

f12_1 = nm.mbest(k="id",s="F_x%nr,time%n",size=1,i=base)
f12_1 <<= nm.mjoin(k="id",m="sumary.csv",f="count")
f12_1 <<= nm.mcal(c="${time}/${count}",a="first_location_of_maximum" ,o="rls12_1.csv")
f12_1.run()

f12_2 = nm.mbest(k="id",s="F_x%n,time%n",size=1,i=base)
f12_2 <<= nm.mjoin(k="id",m="sumary.csv",f="count")
f12_2 <<= nm.mcal(c="${time}/${count}",a="first_location_of_minimum" ,o="rls12_2.csv")
f12_2.run()

f12_3 = nm.mbest(k="id",s="F_x%nr,time%nr",size=1,i=base)
f12_3 <<= nm.mjoin(k="id",m="sumary.csv",f="count")
f12_3 <<= nm.mcal(c="(${time}+1)/${count}",a="last_location_of_maximum" ,o="rls12_3.csv")
f12_3.run()

f12_4 = nm.mbest(k="id",s="F_x%n,time%nr",size=1,i=base)
f12_4 <<= nm.mjoin(k="id",m="sumary.csv",f="count")
f12_4 <<= nm.mcal(c="(${time}+1)/${count}",a="last_location_of_minimum" ,o="rls12_4.csv")
f12_4.run()




#friedrich_coefficients
"""
m_s=[3]
r_s=[30]
for m in m_s:
	for r in r_s:
		f13 =   nm.mslide(k=id,s=time%n,f="F_x:F_xn")
		f13 <<= nm.mcal(c="${F_xn}-${F_x}",a="diff")
		f13 <<= nm.mbucket(k=id,f="F_x:F_x_no",n=r,rng=True) 
		f13 <<= nm.mavg(k="id,F_x_no",f="F_x,diff") 
線形回帰必要 あとまわし
"""


#F_x__has_duplicate
#F_x__has_duplicate_max
#F_x__has_duplicate_min
f14 = nm.mcount(k="id,F_x",a="dcnt",i=base)
f14_1 = nm.mcount(k="id",a="ddcnt",i=f14)
f14_1 <<= nm.mjoin(k="id",f="count",m="sumary.csv")
f14_1 <<= nm.mcal(c="${ddcnt}!=${count}",a="has_duplicate",o="rls14_1.csv") 

f14_2 = nm.mbest(k="id",s="F_x%n",size=1,i=f14)
f14_2 <<= nm.mcal(c="${dcnt}>1",a="has_duplicate_min",o="rls14_2.csv")

f14_3 = nm.mbest(k="id",s="F_x%nr",size=1,i=f14)
f14_3 <<= nm.mcal(c="${dcnt}>1",a="has_duplicate_max",o="rls14_3.csv")

nm.runs([f14_1,f14_2,f14_3])


#index_mass_quantile
q_s=[0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9]

for q in q_s:
	f15_0 = nm.mcal(c="abs(${F_x})",a="F_xabs",i=base)
	f15_1 = f15_0.msum(k="id",f="F_xabs")

	f15 =   nm.maccum(k="id",s="time%n",f="F_xabs:F_xabs_a",i=f15_0)
	f15 <<= nm.mjoin(k="id",f="F_xabs:F_xabs_ttl",m=f15_1)
	f15 <<= nm.mjoin(k="id",f="count",m="sumary.csv")
	f15 <<= nm.mcal(c="(${F_xabs_a}/${F_xabs_ttl})>=%g"%(q),a="mc")
	f15 <<= nm.mbest(k="id",s="mc%nr,time%n",size=1)
	f15 <<= nm.mcal(c="(${time}+1)/${count}",a="index_mass_quantile",o="rls15_%g.csv"%(q))
	f15.run()


#large_standard_deviation
r_s=[0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95]

f16 =  nm.msummary(k="id",f="F_x",c="min,max,sd",i=base)
f16 <<=  nm.mcal(c="${max}-${min}",a="diff")

for r in r_s:
	f16 <<=  nm.mcal(c="${sd}>${diff}*%g"%(r),a="arge_standard_deviation_%g"%(r) )

f16 <<=  nm.writecsv("rls16.csv")
f16.run()


#linear_trend (linregress)
#x=time,y=F_x 
f17_0 = nm.mstats(k="id",i=base,f="time,F_x",c="mean")

f17_1 = nm.msim(k="id",i=base,f="time,F_x",c="covar") # covar Sxy == Syx (r_num)
f17_2 = nm.mstats(k="id",f="time:Sxx,F_x:Syy",c="var",i=base) # Sxx=> time ,Syy F_x 
f17_2 <<= nm.mcal(c="sqrt(${Sxx}*${Syy})",a="r_den")
f17_2 <<= nm.mjoin(k="id",m=f17_1,f="covar")
f17_2 <<= nm.mjoin(k="id",m="sumary.csv",f="count")
f17_2 <<= nm.mjoin(k="id",m=f17_0,f="time:xmean,F_x:ymean")
f17_2 <<= nm.mcal(c="${count}-2",a="df")
f17_2 <<= nm.mcal(c="if(${r_den}==0,0,${covar}/${r_den})",a="rvalue")
f17_2 <<= nm.mcal(c="${covar}/${Sxx}",a="slope")
f17_2 <<= nm.mcal(c="${ymean}-(${slope}*${xmean})",a="intercept")
f17_2 <<= nm.mcal(c="${rvalue}*sqrt(${df}/((1-${rvalue})*(1+${rvalue})))",a="t")
f17_2 <<= nm.mcal(c="sqrt((1-${rvalue}^2)*${Syy}/${Sxx}/${df})",a="stder",o="rls17.csv")
f17_2.run()

#covar=>r_num
#mm=r_den
#prob = 2 * distributions.t.sf(np.abs(t), df) =>pvalue
#distributions.t.sfは
#stdtr(df, t) 
# gamma((df+1)/2)/(sqrt(df*pi)*gamma(df/2)) 
# *integral((1+x**2/df)**(-df/2-1/2), x=-inf..t)
#らしい あとまわし


#longest_strike_above_mean
#longest_strike_below_mean

f18_1 = nm.mjoin(k="id",m="sumary.csv",f="mean",i=base)
f18_1 <<= nm.msortf(f="id,time%n")
f18_1 <<= nm.mcal(c="${mean}<=${F_x}",a="above")
f18_1 <<= nm.mcount(q=True ,k="id,above",a="a_count")
f18_1 <<= nm.mbest(k="id",s="above%nr,a_count%nr",size=1)
f18_1 <<= nm.mcal(c="if(${above}==0,0,${a_count})",a="longest_strike_above_mean",o="rls18_1.csv")
f18_1.run()

f18_2 = nm.mjoin(k="id",m="sumary.csv",f="mean",i=base)
f18_2 <<= nm.msortf(f="id,time%n")
f18_2 <<= nm.mcal(c="${mean}>=${F_x}",a="below")
f18_2 <<= nm.mcount(q=True ,k="id,below",a="b_count")
f18_2 <<= nm.mbest(k="id",s="below%nr,b_count%nr",size=1)
f18_2 <<= nm.mcal(c="if(${below}==0,0,${b_count})",a="longest_strike_below_mean",o="rls18_2.csv")
f18_2.run()

#max_langevin_fixed_point
#friedrich_coefficients必要　あとまわし

#mean_abs_change
#mean_change
f19_1 = nm.mslide(k="id",s="time%n",f="F_x:F_xn",i=base)
f19_1 <<= nm.mcal(c="${F_xn}-${F_x},abs(${F_xn}-${F_x})",a="mean_change,mean_abs_change")
f19_1 <<= nm.mavg(k="id",f="mean_change,mean_abs_change",o="rls19_1.csv")
f19_1.run()
#mean_second_derivative_central
f19_2 = nm.mslide(k="id",s="time%n",f="F_x:F_xn_",t=2,i=base)
f19_2 <<= nm.mcal(c="(${F_xn_2}-2*${F_xn_1}+${F_x})/2",a="mean_second_derivative_central")
f19_2 <<= nm.mavg(k="id",f="mean_second_derivative_central",o="rls19_2.csv")
f19_2.run()


#number_crossing_m
m_s =[-1,0,1]
for m in m_s:
	f20 =   nm.mcal(c="${F_x}>%g"%(m),a="pos",i=base)
	f20 <<= nm.mslide(k="id",s="time%n",f="pos:posN")
	f20 <<= nm.mcal(c="${pos}!=${posN}",a="diffT")
	f20 <<= nm.mcount(k="id,diffT",a="cnt")
	f20 <<= nm.mbest(k="id",s="diffT%nr",size=1)
	f20 <<= nm.mcal(c="if(${diffT}==0,0,${cnt})",a="number_crossing_m",o="rls20_%g.csv"%(m))

	f20.run()


#number_cwt_peaks
# webletは後

#number_peaks
n_s = [1,3,5,10,50]
for n in n_s:
	f21_1 = nm.mslide(k="id",s="time%n",t=n,f="F_x:F_x_a_",i=base)
	f21_2 = nm.mslide(k="id",s="time%n",t=n,f="F_x:F_x_b_",i=base,r=True)
	f21   = nm.mjoin(k="id,time",f="F_x_b_*",i=f21_1,m=f21_2)
	f21 <<= nm.mdelnull(f="F_x_a_*,F_x_b_*")
	f21 <<= nm.mcal(c='max(${F_x_a_*},${F_x_b_*})',a="maxv")
	f21 <<= nm.mcal(c='${F_x}>${maxv}',a="number_peaks")
	f21 <<= nm.msum(k="id",f="number_peaks",o="rls21_%d.csv"%n)
	f21.run()

#partial_autocorrelation
lag_s = [1,2,3,4,5,6,7,8,9]
for lag in lag_s:
	pass
	#f22_0   = nm.mslide(k="id",s="time%n",t=lag , l=True,f="F_x:F_xn",i=base)	
	#f22_1   = nm.msim(k=id,f="F_x,F_xn",c="covar",i=f22_0)
	#f22_2   = nm.mstats(k=id,f="F_x,F_xn",c="var",i=f22_0)
	#f22 = nm.mjoin(k="id",f="F_x:var1,F_xn:var2",i=f22_1,m=f22_2,o="rls22_%d.csv"%(lag))
	#f22.run(msg="on")
	#pacfがいるあと

#percentage_of_reoccurring_datapoints_to_all_datapoints
f23_1 = nm.mcount(k="id,F_x",a="cnt",i=base)
f23_1 <<= nm.mcal(c="${cnt}>1",a="reocur")
f23_1 <<= nm.msummary(k="id",f="reocur",c="sum,count")
f23_1 <<= nm.mcal(c="${sum}/${count}",a="percentage_of_reoccurring_datapoints_to_all_datapoints",o="rls23_1.csv")
f23_1.run()
#percentage_of_reoccurring_values_to_all_values
f23_2 = nm.mcount(k="id,F_x",a="cnt",i=base)
f23_2 <<= nm.mcal(c="if(${cnt}>1,${cnt},0)",a="reocur")
f23_2 <<= nm.msum(k="id",f="reocur")
f23_2 <<= nm.mjoin(k="id",m="sumary.csv",f="count")
f23_2 <<= nm.mcal(c="${reocur}/${count}",a="percentage_of_reoccurring_values_to_all_values",o="rls23_2.csv")

f23_2.run()


qt_s=[0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9]
for qt in qt_s:
	f24_0 = nm.msetstr(a="qtRate",v=qt,i="sumary.csv")
	f24_0 <<= nm.mcal(a="T",c="1-${qtRate}+${count}*${qtRate}")
	f24_0 <<= nm.mcal(a="T1",c="int(${T})")
	f24_0 <<= nm.mcal(a="T2",c="if(fract(${T})==0,${T1},${T1}+1)")
	f24_1 = nm.mnumber(i=base,s="F_x%n",a="qtNo",k="id",S=1)
	f24_1 <<= nm.msortf(f="id,qtNo")
	f24_0 <<= nm.mjoin(k="id,T1",K="id,qtNo",f="F_x:X1",m=f24_1)
	f24_0 <<= nm.mjoin(k="id,T2",K="id,qtNo",f="F_x:X2",m=f24_1)
	f24_0 <<= nm.mcal(a="qt",c="if(${T1}==${T2},${X1},(${T2}-${T})*${X1}+(${T}-${T1})*${X2})",o="rls24_%g.csv"%(qt))
	f24_0.run()

#range_count
mm_s = [[1,-1]]
for max, min in mm_s:
	f25 = nm.mcal(i=base,c="${F_x}>=%g&&${F_x}<%g"%(min,max) ,a="range_count")
	f25 <<= nm.msum(k="id",f="range_count",o="rls25_%g_%g.csv"%(min,max))
	f25.run()


#ratio_beyond_r_sigma
r_s = [0.5,1,1.5,10,2,2.5,3,5,6,7]
for r in r_s:
	f26 =   nm.mjoin(k="id",m="sumary.csv",f="mean,sd,count",i=base)
	f26 <<=	nm.mcal(c="(abs(${F_x}-${mean}))>(%g*${sd})"%(r),a="ratio_beyond_r_sigma_b")
	f26 <<=	nm.msum(k="id",f="ratio_beyond_r_sigma_b")
	f26 <<=	nm.mcal(c="${ratio_beyond_r_sigma_b}/${count}",a="ratio_beyond_r_sigma",o="rls26_%g.csv"%(r))
	f26.run()


#ratio_value_number_to_time_series_length
f27 = nm.msummary(k="id",f="F_x",c="count,ucount",i=base)
f27 <<=  nm.mcal(c="${ucount}/${count}",a="ratio_value_number_to_time_series_length",o="rls27.csv")
f27.run()


#sample_entropy (もとが間違ってるようなきが？)
f28_0 = nm.mslide(k="id",s="time%n",f="F_x:F_xn",i=base)
f28_1 = nm.mslide(k="id",s="time%n",f="F_x:F_xn",l=True,t=2,i=base)

f28_2 = nm.mcombi(k="id",s="time%n",f="time",a="t1,t2",n=2,i=f28_0)
f28_2 <<= nm.mjoin(k="id,t1",K="id,time",m=f28_0,f="F_x:t1_V1,F_xn:t1_V2")
f28_2 <<= nm.mjoin(k="id,t2",K="id,time",m=f28_0,f="F_x:t2_V1,F_xn:t2_V2")
f28_2 <<= nm.mjoin(k="id",m="sumary.csv",f="sd")
f28_2 <<= nm.mcal(c="abs(${t2_V1}-${t1_V1})<${sd}*0.2||abs(${t2_V2}-${t1_V2})<${sd}*0.2",a="B",dlog="Bchk")
f28_2 <<= nm.msum(k="id",f="B")


f28_3 = nm.mcombi(k="id",s="time%n",f="time",a="t1,t2",n=2,i=f28_1)
f28_3 <<= nm.mjoin(k="id,t1",K="id,time",m=f28_1,f="F_x:t1_V1,F_xn:t1_V2")
f28_3 <<= nm.mjoin(k="id,t2",K="id,time",m=f28_1,f="F_x:t2_V1,F_xn:t2_V2")
f28_3 <<= nm.mjoin(k="id",m="sumary.csv",f="sd")
f28_3 <<= nm.mcal(c="abs(${t2_V1}-${t1_V1})<${sd}*0.2||abs(${t2_V2}-${t1_V2})<${sd}*0.2",a="A",dlog="Achk")
f28_3 <<= nm.msum(k="id",f="A")

f28 =   nm.mjoin(k="id",i=f28_2,m=f28_3,f="A")
f28 <<= nm.mcal(c="ln(${A}/${B})*-1",a="sample_entropy",o="rls28.csv")
f28.run()

#spkt_welch_density
# welch　＝＞フーリエ変換必要
#あと


#sum_of_reoccurring_data_points
f29 =   nm.mcount(k="id,F_x",a="cnt",i=base)
f29 <<= nm.mcal(c="if(${cnt}==1,0,${cnt}*${F_x})",a="sum_of_reoccurring_data_points")
f29 <<= nm.msum(k="id",f="sum_of_reoccurring_data_points",o="rls29.csv")
f29.run()

#sum_of_reoccurring_values
f30 =   nm.mcount(k="id,F_x",a="cnt",i=base)
f30 <<= nm.mcal(c="if(${cnt}==1,0,${F_x})",a="sum_of_reoccurring_values")
f30 <<= nm.msum(k="id",f="sum_of_reoccurring_values",o="rls30.csv")
f30.run()

#symmetry_looking
f31_0 =   nm.mcal(c="abs(${mean}-${median}),${max}-${min}",a="mean_median,max_min",i="sumary.csv")
r_s=[0.0,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95]
for r in r_s:
	f31 = nm.mcal(c="${mean_median}<${max_min}*%g"%(r),a="symmetry_looking",i=f31_0,o="rls31_%g.csv"%(r))
	f31.run()
	
#time_reversal_asymmetry_statistic
lag_s = [1,2,3] 
for lag in lag_s:
	f32 =   nm.mslide(i=base,k="id",s="time%n",f="F_x:F_xn",t=lag,l=True)
	f32 <<= nm.mslide(k="id",s="time%n",f="F_xn:F_xnn",t=lag,l=True)
	f32 <<= nm.mcal(c='${F_xnn}^2*${F_xn}-${F_x}^2*${F_xn}',a="time_reversal_asymmetry_statistic")	
	f32 <<= nm.mavg(k="id",f="time_reversal_asymmetry_statistic",o="rls32_%g.csv"%(lag))
	f32.run()	


#value_count
v_s = [0,1] 
for v in v_s:
	f33 = nm.mcal(c='${F_x}==%g'%(v),a="value_count",i=base)
	f33 <<= nm.msum(k="id",f="value_count",o="rls33_%g.csv"%(v))
	f33.run()


#variance_larger_than_standard_deviation
f34= nm.mcal(c='${var}>${sd}',a="variance_larger_than_standard_deviation",i="sumary.csv",o="rls34.csv")
f34.run()

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
