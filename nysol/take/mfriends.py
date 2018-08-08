#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import shutil
import nysol.mcmd as nm
import nysol.util.mtemp as mtemp
import nysol.util.margs as margs


class mfriends(object):
	helpMSG="""
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

	verInfo="version=1.2"
	paramter = {	
		"ni":"filename",
		"nf": "fld",  #<= 使ってない
		"ei":"filename",
		"ef": "fld",
		"eo":"filename",
		"no":"filename",
		"sim": "fld",
		"rank":"int",
		"dir":"str",
		"directed": "bool",
		"udout": "bool",
		"mcmdenv" : "bool",
		"T": "str"
	}

	paramcond = {	
		"hissu": ["ei","sim","eo"]
	}

	def help(self):
		print(mfriends.helpMSG)

	def ver(self):
		print(mfriends.versionInfo)


	def __param_check_set(self , kwd):

		for k,v in kwd.items():
			if not k in mfriends.paramter	:
				raise( Exception("KeyError: {} in {} ".format(k,self.__class__.__name__) ) )
			#型チェック入れる


		self.ei = kwd["ei"]
		self.ni = kwd["ni"]    if "ni"    in kwd else None
		
		# ---- edge field names (two nodes) on ei=
		ef0 = kwd["ef"].split(",")
		self.ef1 = ef0[0]
		self.ef2 = ef0[1] 
		
		# ---- node field name on ni=
		# self.nf  = kwd["nf"] if "nf" in kwd else None

		self.sim  = kwd["sim"] # similarity measure
		self.rank = int(kwd["rank"]) if "rank" in kwd else 3 # ranking 
		self.dir  = kwd["dir"] if "dir" in kwd else "b"  # 方向
		self.directed = kwd["directed"] if "directed" in kwd else False # directed graph
		self.udout    = kwd["udout"] if "udout" in kwd else False # 無向グラフ出力


		if(self.dir!="b" and self.dir!="m" and self.dir!="x"):
			raise Exception("dir= takes b, m, x")

		self.eo      = kwd["eo"]  if "eo"  in kwd else None
		self.no      = kwd["no"]  if "no"  in kwd else None 
		self.logFile = kwd["log"] if "log" in kwd else None 
	
	


	def __init__(self,**kwd):
		#パラメータチェック
		self.__param_check_set(kwd)


	def run(self):
		wf=mtemp.Mtemp()
		xxpal = wf.file()
		xxa   = wf.file()
		xxb   = wf.file()
		xxc   = wf.file()
		xxd   = wf.file()
		xxout = wf.file()

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
		xpal = None
		if self.directed :
			# 任意の枝a->bのaについて上位rankを選択
			xpal <<= nm.mnumber(k=self.ef1,s=self.sim+"%nr",e="skip",S=1,a="##rank",i=self.ei)
			xpal <<= nm.mselnum(f="##rank",c="[,"+str(self.rank)+"]")
		else:
			xxa = nm.mfsort(f=[self.ef1,self.ef2],i=self.ei)
			xxb = nm.mfsort(f=[self.ef2,self.ef1],i=self.ei)
			xpal <<= nm.muniq(k=[self.ef1,self.ef2],i=[xxa,xxb])
			xpal <<= nm.mnumber(k=self.ef1,s=self.sim+"%nr",e="skip",S=1,a="##rank")
			xpal <<= nm.mselnum(f="##rank",c="[,"+str(self.rank)+"]")


		# 両方向+片方向
		xpal1=None
		if self.dir=="x":
			xpal1 <<= nm.mcut(f=[self.ef1,self.ef2,self.sim],i=xpal)
		# 両方向
		elif self.dir=="b":
			selpara = "$s{%s}==$s{##ef2}"%(self.ef1)
			# 得られた上位rankグラフからa->b->cを作成し、a==cであれば相思相愛ということ
			xpal1 <<= nm.mnjoin(k=self.ef2,K=self.ef1,m=xpal,f=self.ef2+":##ef2,"+self.sim+":sim2",i=xpal)
			xpal1 <<= nm.msel(c=selpara)
			xpal1 <<= nm.mcut(f=[self.ef1,self.ef2,self.sim])
		else:
			selpara = "$s{%s}==$s{##ef2}"%(self.ef1)
			xxc = None
			xxc <<= nm.mnjoin(k=self.ef2,K=self.ef1,m=xpal,f=self.ef2+":##ef2,"+self.sim+":sim2",i=xpal)
			xxc <<= nm.msel(c=selpara)
			xxc <<= nm.mcut(f=[self.ef1,self.ef2])
			xpal1 <<= nm.mcut(f=[self.ef1,self.ef2,self.sim],i=xpal)
			xpal1 <<= nm.mcommon(k=self.ef1+","+self.ef2,m=xxc,r=True)

		runpal =None
		kpara="%s,%s"%(self.ef1,self.ef2)
		if self.udout :
			runpal <<= nm.mfsort(f=kpara,i=xpal1)
			runpal <<= nm.mavg(k=kpara,f=self.sim)
			runpal <<= nm.msortf(f=kpara,o=self.eo)
		else:
			runpal <<= nm.msortf(f=kpara,i=xpal1,o=self.eo)

		runpal.run()

		if self.ni and self.no :
			shutil.copyfile(self.ni,self.no)


