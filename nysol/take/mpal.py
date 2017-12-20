#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import shutil
import nysol.mod as nm
import nysol.util.margs as margs
import nysol.util.mtemp as mtemp
import nysol.take.mtra2gc as mtra2gc  
import nysol.take.mfriends as mfriends


class mpal(object):
	def help(self):
		"""
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
	def ver(self):
		print("version #{$version}")

	def __init__(self,args):
		self.args = args

		# mcmdのメッセージは警告とエラーのみ
		#ENV["KG_VerboseLevel"]="2" unless args.bool("-mcmdenv")

		#ワークファイルパス
		#if args.str("T=")!=nil then
		#	ENV["KG_TmpPath"] = args.str("T=").sub(/\/$/,"")
		#end
		self.iFile  = args.str("i=")
		self.idFN   = args.str("tid=")
		self.itemFN = args.str("item=")
		self.sp1    = args.str("s=")
		self.sp2    = args.str("S=")
		#node_support=args.bool("-node_support")
		self.numtp   = args.bool("-num")

		self.filter= args.str("filter=",[],",")
		self.ub    = args.str("ub="  ,[],",")
		self.lb    = args.str("lb="  ,[],",")

		self.sim   = args.str("sim=" ,"S",",")
		self.dir   = args.str("dir=" ,"b",",")
		self.rank  = args.str("rank=","3",",")
		self.prune = args.bool("-prune")

		self.filterStr=[None]*3
		self.lbStr=["0"]*3
		self.ubStr=["1"]*3

		# chicking parameter for filter=
		if len(self.filter)>0:

			#if not self.lb:
			#	raise Exception("lb= have to be set with filter=")
		
			for i in range(len(self.filter)-1):
				for j in range(1,len(self.filter)):
					if i==j :
						continue
					if self.filter[i]==self.filter[j]:
						raise Exception("filter= cannot take same values")
					
			if len(self.filter) > 3:
				raise Exception("flter=,lb=,ub= takes parameters with 0<size<=3")
				
			for s in self.filter:
				if s!="J" and s!="P" and s!="C" :
					raise Exception("filter= takes J, P or C")
				
			for i in range(len(self.filter)):
				if self.filter[i] == "J":
					self.filterStr[i] = "jaccard"
				elif self.filter[i] == "P":
					self.filterStr[i] = "PMI"
				elif self.filter[i] == "C":
					self.filterStr[i] = "confidence"
				else:
					raise Exception("filter= takes J, P or C")
				
				if self.lb and len(self.lb) > i :
					self.lbStr[i] = self.lb[i]

				if self.ub and len(self.ub) > i :
					self.ubStr[i] = self.ub[i]


		# chicking parameter for sim=
		for i in range(len(self.sim)-1):
				for j in range(1,len(self.sim)):
					if i==j:
						continue
					if self.sim[i]==self.sim[j] :
						raise Exception("sim= cannot take same values")

		if len(self.sim)>3 :
			raise Exception("sim=,dir=,rank= takes parameters with 0<size<=3")

		if not ( len(self.sim)==len(self.dir) and len(self.dir) == len(self.rank)):
			raise Exception("sim=,dir=,rank= must take same size of parameters")

		for s in self.sim:
			if s!="S" and s!="J" and s!="P" and s!="C" :
				raise Exception("sim= takes S, J, P or C")

		for s in self.dir:
			if s!="b" and s!="m" and s!="x" :
				raise Exception("dir= takes b, m, x")

		for s in self.rank:
			if int(s) < 0 :
				raise Exception("rank= takes positive integer")

		
		self.simStr=[]
		for s in self.sim :
			if s=="S":
				self.simStr.append("support")
			elif s=="J":
				self.simStr.append("jaccard")
			elif s=="P":
				self.simStr.append("PMI")
			elif s=="C":
				self.simStr.append("confidence")
			else:
				raise Exception("sim= takes S, J, P or C")
				
		self.orFile  = args. file("ro=", "w")
		self.onFile  = args. file("no=", "w")
		self.oeFile  = args. file("eo=", "w")


	# ============
	# entry point
	def run(self):
		temp=mtemp.Mtemp()
		xxsimgN=temp.file()
		xxsimgE0=temp.file()
		xxsimgE=temp.file()
		xxfriendE=temp.file()
		xxrecs2=temp.file()
		xxfriends=temp.file()
		xxw=temp.file()
		xxf=temp.file()
		xxff=temp.file()
		xxor=temp.file()

		### mtra2gc
		param = []
		param.append("mtra2gc")
		param.append("i="+self.iFile)
		if self.idFN:
			param.append("tid="+self.idFN)
		if self.itemFN:
			param.append("item="+self.itemFN)
		if self.sp1:
			param.append("s="+self.sp1)
		if self.sp1:
			param.append("S="+self.sp2)


		#####################
		# 異なる向きのconfidenceを列挙するためにsim=C th=0として双方向列挙しておく
		# 出力データは倍になるが、mfriendsで-directedとすることで元が取れている
		param.append("sim=C")
		param.append("th=0")
		#####################
		param.append("-node_support")
		if self.numtp :
			param.append("-num")
		param.append("no="+xxsimgN)
		param.append("eo="+xxsimgE0)

		args=margs.Margs(param)
		mtra2gc.mtra2gc(args).run()


		if len(self.filter)>0:
			f = nm.mselnum(i=xxsimgE0,f=self.filterStr[0],c="[%s,%s]"%(self.lbStr[0],self.ubStr[0]))

			if self.filterStr[1] :
				f <<= nm.mselnum(f=self.filterStr[1],c="[%s,%s]"%(self.lbStr[1],self.ubStr[1]))

			if self.filterStr[2] :
				f <<= nm.mselnum(f=self.filterStr[1],c="[%s,%s]"%(self.lbStr[2],self.ubStr[2]))

			f.writecsv(xxsimgE).run()

		else:
			shutil.move(xxsimgE0,xxsimgE)

		col=[ ["FF000080","FF888880"], ["0000FF80","8888FF80"], ["00FF0080","88FF8880"]]


		if not os.path.isdir(xxfriends) :
			os.makedirs(xxfriends)

		# friendの結果dir
		for i in range(len(self.sim)):
			param =[]
			param.append("mfriends")
			param.append("ei="+xxsimgE)
			param.append("ni="+xxsimgN)
			param.append("ef=node1,node2")
			param.append("nf=node")
			param.append("sim="+self.simStr[i])
			param.append("-directed")
			if self.dir[i] :
				param.append("dir="+self.dir[i])
			param.append("rank="+self.rank[i])
			param.append("eo="+xxfriendE)
			param.append("no="+xxfriends+"/n_"+str(i))

			args=margs.Margs(param)
			mfriends.mfriends(args).run()
			
			f = nm.mfsort(f="node1,node2",i=xxfriendE)
			f <<= nm.msummary(k="node1,node2",f=self.simStr[i],c="count,mean")
			# node1%0,node2%1,fld,count,mean
			# a,b,support,2,0.1818181818
			# a,d,support,2,0.1818181818
			f <<= nm.mselstr(f="count",v=2,o=xxrecs2)
			f.run()


			f = nm.mjoin(k="node1,node2",K="node1,node2",m=xxrecs2,f="mean:s1",n=True,i=xxfriendE)
			f <<= nm.mjoin(k="node2,node1",K="node1,node2",m=xxrecs2,f="mean:s2",n=True)
			# 1) xxrecs2でsimをjoinできない(s1,s2共にnull)ということは、それは片方向枝なので"F"をつける
			# 2) 双方向枝a->b,b->aのうちa->bのみ(s1がnullでない)に"W"の印をつける。
			# 3) それ以外の枝は"D"として削除
			f <<= nm.mcal(c='if(isnull($s{s1}),if(isnull($s{s2}),\"F\",\"D\"),\"W\")',a="dir")
			f <<= nm.mselstr(f="dir",v="D",r=True)
			f <<= nm.mcal(c='if($s{dir}==\"W\",$s{s1},$s{%s})'%(self.simStr[i]),a="sim")
			f <<= nm.mchgstr(f="dir:color",c='W:%s,F:%s'%(col[i][0],col[i][1]),A=True)
			f <<= nm.msetstr(v=self.simStr[i]+","+str(i),a="simType,simPriority")
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
			# 双方向と片方向に分割
			nm.mcat(i=xxfriends+"/e_*").mselstr(f="dir",v="W",o=xxw,u=xxf).run()
			# 片方向のみの枝を選択
			f =   nm.mcommon(k="node1,node2",K="node1,node2",r=True,m=xxw,i=xxf)
			f <<= nm.mcommon(k="node1,node2",K="node2,node1",r=True,m=xxw,o=xxff)
			f.run()
			f = nm.mcat(i=xxw+","+xxff).mbest(k="node1,node2",s="dir%r,simPriority%n",o=self.oeFile).run()
			
		else:
			nm.mcat(i=xxfriends+"/e_*",o=self.oeFile).run()

		shutil.move(xxfriends+"/n_0",self.onFile)


		# end message
		#MCMD::endLog(args.cmdline)


if __name__ == '__main__':

	import sys
	args=margs.Margs(sys.argv,"i=,tid=,item=,ro=,eo=,no=,s=,S=,filter=,lb=,ub=,sim=,dir=,rank=,-prune,-num,-verbose","i=,tid=,item=,eo=,no=")
	mpal(args).run()

