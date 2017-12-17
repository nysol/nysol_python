#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import nysol.mod as nm
import mtemp as nutil
import margs as nu


class mfriends(object):
	def help(self):
		"""
# ver="1.0" # 初期リリース 2016/10/2
# ver="1.1" -pal,rank2=,-udout追加 2016/12/13
# ver="1.2" rank2=削除,-palの意味を変更 2016/12/25
$cmd=$0.sub(/.*\//,"")
$version="1.2"
----------------------------
#{$cmd} version #{$version}
----------------------------
概要) 相互類似関係にある枝を選択する。
内容) グラフG=(V,E)の任意の枝(a,b)∈E について条件b∈A and a∈Bを満たす枝を選択する。
      ここで、A(B)は節点a(b)の隣接節点の内、類似度が上位r個の節点集合のこと。
      rはrank=で指定し、類似度(項目)はsim=で指定する。
      結果は有向グラフとして出力される。例えば節点a,bが相互類似関係にあれば、a->b,b->aの両枝が出力される。
書式) #{$cmd} ei= [ef=] [ni=] [nf=] eo= [no=] [sim=] [dir=b|m|x] [-directed] [T=] [--help]

  ファイル名指定
  ei=   : 枝データファイル
  ef=   : 枝データ上の2つの節点項目名(省略時は"node1,node2")
  ni=   : 節点データファイル
  nf=   : 節点データ上の節点項目名(省略時は"node")
  eo=   : データ研磨後の枝データファイル
  no=   : データ研磨後の節点データファイル
  rank= : 類似度上位何個までの隣接節点を対象とするか(省略時は3)
  sim=  : rank=で使う節点間類似度(枝の重み)項目名。
  dir=  : b:双方向類似枝のみ出力する(デフォルト)
        : m:片方向類似枝のみ出力する
        : x:双方向類似枝、片方向類似枝両方共出力する。
  -directed: 有向グラフとみなして計算する。
  -udout: 無向グラフとして出力する。両方向に枝がある場合(a->b,b->a)の枝はa-bとして出力される。
          a->b,b->aで類似度が異なる場合は平均値が出力される。

  その他
  T= : ワークディレクトリ(default:/tmp)
  --help : ヘルプの表示

例) 基本例
$ cat edge.csv
n1,n2,sim
a,b,0.40
a,c,0.31
a,d,0.31
b,c,0.20
b,d,0.24
b,e,0.14
c,d,0.30
d,e,0.09
$ #{$cmd} ei=edge.csv ef=n1,n2 sim=sim rank=2 eo=output.csv
#END# #{$cmd} ei=edge.csv ef=n1,n2 sim=sim rank=2 eo=output.csv; 2016/10/02 09:58:22
$ cat output.csv
n1%0,n2%1,sim
a,b,0.40
a,c,0.31
a,d,0.31
b,a,0.40
c,a,0.31
c,d,0.30
d,a,0.31
d,c,0.30

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

		self.ei = args.file("ei=","r") # edge file name
		self.ni = args.file("ni=","r") # node file name

		# ---- edge field names (two nodes) on ei=
		ef0 = args.field("ef=", self.ei, None,2,2)["names"]
		self.ef1 =ef0[0]
		self.ef2 =ef0[1] 

		# ---- node field name on ni=
		self.nf = args.field("nf=", self.ni, None,1,1)
		if self.nf :
			self.nf = self.nf["names"][0]

		self.sim  		= args.field("sim=",self.ei,None,1,1)["names"][0]    # similarity measure
		self.rank     = args.int("rank=",3)     # ranking 
		self.dir      = args.str("dir=","b")    # 方向
		self.directed = args.bool("-directed")  # directed graph
		self.udout    = args.bool("-udout")     # 無向グラフ出力

		if(self.dir!="b" and self.dir!="m" and self.dir!="x"):
			raise Exception("dir= takes b, m, x")

		self.eo      = args.file("eo=", "w")
		self.no      = args.file("no=", "w")
		self.logFile = args.file("log=", "w")


	def run(self):
		wf=nutil.Mtemp()
		xxpal   =wf.file()
		xxa=wf.file()
		xxb=wf.file()
		xxc=wf.file()
		xxd=wf.file()
		xxout=wf.file()

		# ============
		# n1,n2,sim
		# a,b,0.40
		# a,c,0.31
		# a,d,0.22
		# b,c,0.20
		# b,d,0.24
		# b,e,0.14
		# c,d,0.30
		# d,e,0.09
		if self.directed :
			# 任意の枝a->bのaについて上位rankを選択
			nm.mnumber(k=self.ef1,s=self.sim+"%nr",e="skip",S=1,a="##rank",i=self.ei).mselnum(f="##rank",c="[,"+str(self.rank)+"]",o=xxpal).run()
		else:
			nm.mfsort(f=self.ef1+","+self.ef2,i=self.ei,o=xxa).run()
			nm.mfsort(f=self.ef2+","+self.ef1,i=self.ei,o=xxb).run()
			nm.mcat(i=xxa+","+xxb).muniq(k=self.ef1+","+self.ef2).mnumber(k=self.ef1,s=self.sim+"%nr",e="skip",S=1,a="##rank").mselnum(f="##rank",c="[,"+str(self.rank)+"]",o=xxpal).run()


		# 両方向+片方向
		if self.dir=="x":
			nm.mcut(f=self.ef1+","+self.ef2+","+self.sim,i=xxpal,o=xxout).run()
		# 両方向
		elif self.dir=="b":
			selpara = "$s{%s}==$s{##ef2}"%(self.ef1)
			# 得られた上位rankグラフからa->b->cを作成し、a==cであれば相思相愛ということ
			nm.mnjoin(k=self.ef2,K=self.ef1,m=xxpal,f=self.ef2+":##ef2,"+self.sim+":sim2",i=xxpal).msel(c=selpara).mcut(f=self.ef1+","+self.ef2+","+self.sim,o=xxout).run()
		else:
			selpara = "$s{%s}==$s{##ef2}"%(self.ef1)
			nm.mnjoin(k=self.ef2,K=self.ef1,m=xxpal,f=self.ef2+":##ef2,"+self.sim+":sim2",i=xxpal).msel(c=selpara).mcut(f=self.ef1+","+self.ef2,o=xxc).run()
			nm.mcut(f=self.ef1+","+self.ef2+","+self.sim,i=xxpal).mcommon(k=self.ef1+","+self.ef2,m=xxc,r=True,o=xxout).run()

		kpara="%s,%s"%(self.ef1,self.ef2)
		if self.udout :
			nm.mfsort(f=kpara,i=xxout).mavg(k=kpara,f=self.sim).msortf(f=kpara,o=self.eo).run()
		else:
			#os.system("cat %s > %s_1"%(xxout,self.eo))
			f = nm.msortf(f=kpara,i=xxout,o=self.eo)
			f.run()

		if self.ni and self.no :
			os.system("cp %s %s"%(self.ni,self.no))
		# 終了メッセージ
		#MCMD::endLog(args.cmdline)


if __name__ == '__main__':

	import sys
	args=nu.Margs(sys.argv,"ni=,nf=,ei=,ef=,eo=,no=,sim=,rank=,dir=,-directed,-udout","ei=,ef=,sim=")
	mfriends(args).run()
