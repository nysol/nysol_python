#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import shutil
import nysol.mcmd as nm
import nysol.util.margs as margs
import nysol.util.mtemp as mtemp
from nysol import take as nt


class mpal(object):
	helpMSG="""
# 1.0 initial development: 2016/12/23
# 1.1 dir=x追加: 2016/12/29
# 1.2 jac=等追加: 2017/1/10
# 1.3 ro=追加: 2017/1/15
$version="1.3"
----------------------------
#{$cmd} version #{$version}
----------------------------
概要) トランザクションデータから２アイテム相関ルールを求め、ランクに基づいた類似関係にある相関ルールを選択する

書式) #{$cmd} i= tid= item= [class=] [ro=] [no=] eo= s=|S= [filter=] [lb=] [ub=] [sim=] [rank=] [dir=] [-prune] [T=] [--help]
  i=     : トランザクションデータファイル【必須】
  tid=   : トランザクションID項目名【必須】
  item=  : アイテム項目名【必須】
  ro=    : 出力ルールファイル
  no=    : 出力ファイル(節点)
  eo=    : 出力ファイル(辺:節点ペア)
  s=     : 最小支持度(全トランザクション数に対する割合による指定): 0以上1以下の実数
  S=     : 最小支持度(トランザクション数による指定): 1以上の整数
  filter=: 相関ルールの評価指標(省略可,複数指定可)
           指定できる類似度は以下の4つ(括弧内は値域)。
             J: Jaccard(0..1), P:normalized PMI(-1..0..1), C:Confidence(0..1)
  lb=    : filter=で指定した相関ルール評価指標の下限値
  ub=    : filter=で指定した相関ルール評価指標の上限値
           lb=が省略された場合のデフォルトで0、ub=のデフォルトは1
  例1: sim=P lb=0.5 : normalized PMIが0.5以上1以下の相関ルールを列挙する
  例2: sim=P,C lb=0.5,0.2 ub=,0.8 : 例1に加えて、confidenceが0.2以上0.8以下の相関ルールを列挙する

  sim=   : 列挙された相関ルールを元にして枝を張る条件となる指標を指定する。
           以下に示す4つの相関ルール評価指標が指定できる(デフォルト:S)。
             S:Support, J: Jaccard, P:normalized PMI, C:Confidence
  rank=  : 枝を張る条件で、類似枝の上位何個までを選択するか(デフォルト:3)
  dir=   : 双方向類似関係(b)のみを出力するか、単方向類似関係(m)のみか、両方とも含める(x)かを指定する。(デフォルト:b)
           相関ルールa=>bの類似度をsim(a=>b)としたとき、
           b:(bi-directional) sim(a=>b)およびsim(b=>a)がrank=で指定した順位内である相関ルールのみ選択される。
           m:(mono-directional) 片方向の類似度のみが、指定された順位内である相関ルールが選択される。
           x:(both) bとmの両方共含める。
  以上の3つのパラメータは複数指定することが可能(3つまで)。
  例1: sim=S dir=b rank=3 :
         アイテムaからみてsupport(a=>b)が3位以内で、かつ
         アイテムbからみてsupport(b=>a)も3位以内であるような相関ルールを選択する
  例2: sim=S,C dir=b,m rank=3,1
         例1に加えて、アイテムaから見てconfidenc(a=>b)が3以内、もしくは
         アイテムbから見てconfidenc(b=>a)が3以内であれば、そのような相関ルールも選択する
  -prune : sim=等を複数指定した場合、マルチ枝を単一化する。
           第1優先順位: 双方向>片方向
           第2優先順位: パラメータ位置昇順

  その他
  T= : ワークディレクトリ(default:/tmp)
  --help : ヘルプの表示

入力ファイル形式)
トランザクションIDとアイテムの２項目によるトランザクションデータ。

o=の出力形式)
枝ファイル: simType,simPriority,node1,node2,sim,dir,color
節点ファイル: node,support,frequency,total

例)
$ cat tra.csv
id,item
1,a
1,b
2,a
2,b
3,a
3,b
4,b
4,c
5,a
5,c
6,a
6,c
7,d
7,e
8,d
8,e
9,d
9,e
A,d
A,c
B,e
B,b
C,e
C,a
D,f
D,c
E,f
E,b
F,f
F,a

$ #{$cmd} i=tra1.csv no=node11.csv eo=edge11.csv tid=id S=1 item=item sim=S rank=1 dir=b
$ cat edge11.csv 
simType,simPriority,node1,node2,sim,dir,color
support,0,a,e,0.2727272727,W,FF0000
support,0,d,e,0.2727272727,W,FF0000
support,0,c,f,0.1818181818,W,FF0000
support,0,g,h,0.1818181818,W,FF0000

$ #{$cmd} i=tra2.csv no=node51.csv eo=edge51.csv tid=id S=1 item=item sim=S,C rank=1,1 dir=b,m -prune
$ cat edge51.csv 
simType,simPriority%3n,node1%0,node2%1,sim,dir%2r,color
support,0,a,b,0.2,W,FF0000
confidence,1,c,a,0.4,F,8888FF
support,0,d,e,0.2,W,FF0000
confidence,1,f,a,0.3333333333,F,8888FF
confidence,1,f,b,0.3333333333,F,8888FF
confidence,1,f,c,0.3333333333,F,8888FF

# Copyright(c) NYSOL 2012- All Rights Reserved.
"""

	paramter = {	
		"i": "filename",
		"tid": "fld",
		"item":"fld",
		"ro": "filename",
		"eo": "filename",
		"no": "filename",
		"s" : "float",
		"S":"int",
		"filter":"str",
		"lb":"int",
		"ub":"int",
		"sim": "str",
		"dir" :"str",
		"rank":"fld",
		"prune" : "bool",
		"num": "bool",
		"verbose":"bool",
		"T": "str"
	}
	paramcond = {	
		"hissu":["i","tid","item","eo","no"]
	}
	

	def help():
		print(mpal.helpMSG) 

	def ver():
		print(mpal.versionInfo)

	
	def __param_check_set(self , kwd):
		for k,v in kwd.items():
			if not k in mpal.paramter	:
				raise( Exception("KeyError: {} in {} ".format(k,self.__class__.__name__) ) )
			#型チェック入れる

		self.iFile   = kwd["i"]
		self.idFN    = kwd["tid"]
		self.itemFN  = kwd["item"]
		self.onFile  = kwd["no"]
		self.oeFile  = kwd["eo"]

		self.orFile  =  kwd["ro"] if "ro" in kwd else None
				
		self.sp1    = kwd["s"] if "s" in kwd else None
		self.sp2    = kwd["S"] if "S" in kwd else None


		self.numtp = kwd["num"]   if "num"   in kwd else False
		self.prune = kwd["prune"] if "prune" in kwd else False 

		self.dir   = kwd["dir"].split(",")  if "dir"  in kwd else ["b"]
		self.rank  = kwd["rank"].split(",") if "rank" in kwd else ["3"]

		_sim    = kwd["sim"].split(",")    if "sim"  in kwd else ["S"]

		_filter = kwd["filter"].split(",") if "filter" in kwd else [] 
		_ub     = kwd["ub"].split(",")     if "ub" in kwd else [] 
		_lb     = kwd["lb"].split(",")     if "lb" in kwd else [] 

		self.filter=[None]*3
		self.lb=["0"]*3
		self.ub=["1"]*3

		self.sim=[]

		self.filterSize = len(_filter)

		# chicking parameter for filter=
		if len(_filter)>0:
			if len(_filter) != len(set(_filter)):
				raise Exception("filter= cannot take same values")
					
			if len(_filter) > 3:
				raise Exception("flter=,lb=,ub= takes parameters with 0<size<=3")
				
			for s in _filter:
				if s!="J" and s!="P" and s!="C" :
					raise Exception("filter= takes J, P or C")
			
			# dictに
			for i in range(len(_filter)):
				if _filter[i] == "J":
					self.filter[i] = "jaccard"
				elif _filter[i] == "P":
					self.filter[i] = "PMI"
				elif _filter[i] == "C":
					self.filter[i] = "confidence"
				else:
					raise Exception("filter= takes J, P or C")
				
				if len(_lb) > i :
					self.lb[i] = _lb[i]

				if len(_ub) > i :
					self.ub[i] = _ub[i]


		# chicking parameter for sim=
		if len(_sim) != len(set(_sim)):
			raise Exception("sim= cannot take same values")

		if len(_sim)>3 :
			raise Exception("sim=,dir=,rank= takes parameters with 0<size<=3")

		if not ( len(_sim)==len(self.dir) and len(self.dir) == len(self.rank)):
			raise Exception("sim=,dir=,rank= must take same size of parameters")

		for s in _sim:

			if s=="S":
				self.sim.append("support")
			elif s=="J":
				self.sim.append("jaccard")
			elif s=="P":
				self.sim.append("PMI")
			elif s=="C":
				self.sim.append("confidence")
			else:
				raise Exception("sim= takes S, J, P or C")

		for s in self.dir:
			if s!="b" and s!="m" and s!="x" :
				raise Exception("dir= takes b, m, x")

		for s in self.rank:
			if int(s) < 0 :
				raise Exception("rank= takes positive integer")


	def __init__(self,**kwd):

		#パラメータチェック
		self.args = kwd
		self.__param_check_set(kwd)



	# ============
	# entry point
	def run(self):
		temp=mtemp.Mtemp()

		### mtra2gc
		xxsimgN=temp.file()
		xxsimgE=temp.file()
		xxsimgE0=temp.file()

		param = {}
		param["i"] = self.iFile
		if self.idFN:
			param["tid"]  = self.idFN
		if self.itemFN:
			param["item"] = self.itemFN
		if self.sp1:
			param["s"] = self.sp1
		if self.sp2:
			param["S"] = self.sp2

		#####################
		# 異なる向きのconfidenceを列挙するためにsim=C th=0として双方向列挙しておく
		# 出力データは倍になるが、mfriendsで-directedとすることで元が取れている
		param["sim"]="C"
		param["th"]= "0"

		param["node_support"] = True
		if self.numtp :
			param["num"] = True
		param["no"] = xxsimgN
		param["eo"] = xxsimgE0

		nt.mtra2gc(**param).run()

		f=nm.readcsv(xxsimgE0)
		for i in range(self.filterSize):
			f <<= nm.mselnum(f=self.filter[i],c="[%s,%s]"%(self.lb[i],self.ub[i]))
		f <<= nm.writecsv(xxsimgE)
		f.run()


		### mfrirends
		xxfriends=temp.file()
		xxfriendE=temp.file()
		xxw=temp.file()
		xxf=temp.file()
		xxff=temp.file()
		xxor=temp.file()

		if not os.path.isdir(xxfriends) :
			os.makedirs(xxfriends)
		col=[ ["FF000080","FF888880"], ["0000FF80","8888FF80"], ["00FF0080","88FF8880"]]

		for i in range(len(self.sim)):
			paramf ={}
			paramf["ei"] = xxsimgE
			paramf["ni"] = xxsimgN
			paramf["ef"] = "node1,node2"
			paramf["nf"] = "node"
			paramf["eo"] = xxfriendE
			paramf["no"] = xxfriends+"/n_"+str(i)
			paramf["sim"]  = self.sim[i]
			paramf["dir"]  = self.dir[i]
			paramf["rank"] = self.rank[i]
			paramf["directed"] = True

			nt.mfriends(**paramf).run()
			
			frec2 = nm.mfsort(f="node1,node2",i=xxfriendE)
			frec2 <<= nm.msummary(k="node1,node2",f=self.sim[i],c="count,mean")
			frec2 <<= nm.mselstr(f="count",v=2)
			# node1%0,node2%1,fld,count,mean
			# a,b,support,2,0.1818181818
			# a,d,support,2,0.1818181818

			f = nm.mjoin(k="node1,node2",K="node1,node2",m=frec2,f="mean:s1",n=True,i=xxfriendE)
			f <<= nm.mjoin(k="node2,node1",K="node1,node2",m=frec2,f="mean:s2",n=True)
			# 1) xxrecs2でsimをjoinできない(s1,s2共にnull)ということは、それは片方向枝なので"F"をつける
			# 2) 双方向枝a->b,b->aのうちa->bのみ(s1がnullでない)に"W"の印をつける。
			# 3) それ以外の枝は"D"として削除
			f <<= nm.mcal(c='if(isnull($s{s1}),if(isnull($s{s2}),\"F\",\"D\"),\"W\")',a="dir")
			f <<= nm.mselstr(f="dir",v="D",r=True)
			f <<= nm.mcal(c='if($s{dir}==\"W\",$s{s1},$s{%s})'%(self.sim[i]),a="sim")
			f <<= nm.mchgstr(f="dir:color",c='W:%s,F:%s'%(col[i][0],col[i][1]),A=True)
			f <<= nm.msetstr(v=[self.sim[i],str(i)],a="simType,simPriority")
			f <<= nm.mcut(f="simType,simPriority,node1,node2,sim,dir,color",o=xxfriends+"/e_"+str(i))
			f.run()
			# node1%1,node2%0,simType,sim,dir,color
			# b,a,jaccard,0.3333333333,F,8888FF
			# j,c,jaccard,0.3333333333,F,8888FF
			# b,d,jaccard,0.3333333333,F,8888FF
			# a,e,jaccard,0.5,W,0000FF
			# d,e,jaccard,0.5,W,0000FF


		# rule fileの出力
		if self.orFile:
			mmm=nm.mcat(i=xxfriends+"/e_*").muniq(k="node1,node2")
			nm.mcommon(k="node1,node2",i=xxsimgE , m=mmm, o=self.orFile).run()

		# マルチ枝の単一化(W優先,パラメータ位置優先)
		if self.prune:
			"""
			# 双方向と片方向に分割
			nm.mcat(i=xxfriends+"/e_*").mselstr(f="dir",v="W",o=xxw,u=xxf).run()
			# 片方向のみの枝を選択
			f =   nm.mcommon(k="node1,node2",K="node1,node2",r=True,m=xxw,i=xxf)
			f <<= nm.mcommon(k="node1,node2",K="node2,node1",r=True,m=xxw,o=xxff)
			f.run()
			f = nm.mcat(i=xxw+","+xxff).mbest(k="node1,node2",s="dir%r,simPriority%n",o=self.oeFile).run()
			"""
			#これだめ
			fo = nm.mcat(i=xxfriends+"/e_*").mselstr(f="dir",v="W")
			fu = fo.direction("u") # これは再考
			fu <<= nm.mcommon(k="node1,node2",K="node1,node2",r=True,m=fo)
			fu <<= nm.mcommon(k="node1,node2",K="node2,node1",r=True,m=fo)
			#f  =   nm.m2cat()
			f  = nm.mbest(i=[fo,fu],k="node1,node2",s="dir%r,simPriority%n",o=self.oeFile)

			f.run()



		else:
			nm.mcat(i=xxfriends+"/e_*",o=self.oeFile).run()

		nm.mcat(i=xxfriends+"/n_0",o=self.onFile).run()


