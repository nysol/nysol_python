#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import os.path
import shutil

import nysol.mcmd as nm
import nysol.util as nu


class mgdiff(object):

	helpMSG="""
----------------------------
#{$cmd} version #{$version}
----------------------------
概要) ２つのグラフを比較し、同じ枝と異なる枝の情報を出力する。
書式) #{$cmd} ei= [ef=] eI= [eF=] [eo=] [no=] [T=] [--help]

  ファイル名指定
  ei=  : 入力枝ファイル1
  ef=  : 枝を構成するの2つの節点項目名(ei=上の項目名,デフォルト:node1,node2)
  eI=  : 入力枝ファイル2
  eF=  : eI=上の2つの節点項目名(ef=と同じであれば省略できる,デフォルト:ef=で指定した項目名)

  ni=  : 入力節点ファイル1(ei=に対応)
  nf=  : 枝を構成するの2つの節点項目名(ni=上の項目名,デフォルト:node)
  nI=  : 入力節点ファイル2(eI=に対応)
  nF=  : nI=上の2つの節点項目名(nf=と同じであれば省略できる,デフォルト:nf=で指定した項目名)

  eo=  : 出力枝ファイル
  no=  : 出力節点ファイル

	-dir : 有向グラフとして扱う

  その他
  T= : ワークディレクトリ(default:/tmp)
  --help : ヘルプの表示

  注1) 無向グラフとして扱う場合(デフォルト)、ei=ファイルとeI=ファイルとで、
       節点の並びが異なっていても、それは同じと見なす(ex. 枝a,bと枝b,aは同じ)。
       -dirを指定すれば異なるものと見なす。
  注2) 無向グラフとして扱う場合(デフォルト)、
       処理効率を重視し、ef=で指定した節点の並びはアルファベット順に並べ替えるため、
       eo=の項目の並びがei=やeI=の並びと異なることがある。
  注3) 同じ枝が複数ある場合、それらは単一化される。

入力データ)
ei=,eI=: 節点ペアからなるCSVファイル。
ni=,nI=: 節点からなるCSVファイル。

枝出力データ)
枝ファイル1と枝ファイル2のいずれかに出現する枝(節点ペア)について以下の値を出力する。
項目名: 内容
 ei    : ei=で指定したグラフにその行の節点ペアがあれば、そのファイル名
 eI    : eI=で指定したグラフにその行の節点ペアがあれば、そのファイル名
 diff : 差分の区分
         1: ei=のグラフにしか存在しない
         0: ei=,eI=の両方に存在する
        -1: eI=のグラフにしか存在しない

節点出力データ)
節点ファイル1と節点ファイル2のいずれかに出現する節点について以下の値を出力する。
項目名: 内容
 ni    : ni=で指定したグラフにその節点があれば、そのファイル名
 nI    : nI=で指定したグラフにその節点があれば、そのファイル名
 diff : 差分の区分
         1: ni=のグラフにしか存在しない
         0: ni=,nI=の両方に存在する
        -1: nI=のグラフにしか存在しない


例)
$ cat g1.csv
node1,node2
a,b
b,c
c,d

$ cat g2.csv
node1,node2
b,a
c,d
d,e

$ mgdiff.py ei=g1.csv eI=g2.csv ef=node1,node2
node1,node2,i,I,diff
a,b,g1.csv,g2.csv,0
b,c,g1.csv,,1
c,d,g1.csv,g2.csv,0
d,e,,g2.csv,-1

$ mgdiff.py ei=g1.csv eI=g2.csv ef=node1,node2 -dir
node1,node2,i,I,diff
a,b,data/g1.csv,,1
b,a,,data/g2.csv,-1
b,c,data/g1.csv,,1
c,d,data/g1.csv,data/g2.csv,0
d,e,,data/g2.csv,-1

# Copyright(c) NYSOL 2012- All Rights Reserved.
	"""

	verInfo="version=1.2"

	paramter = {	
		"ei":"filename",
		"ef":"fld",
		"eI":"filename",
		"eF":"fld",
		"ni":"filename",
		"nf":"fld",
		"nI":"filename",
		"nF":"fld",
		"eo":"filename",		
		"no":"filename",
		"dir":"bool",
		"T":"str"
	}

	paramcond = {	
		"hissu": ["ei","eI","eo"]
	}

	def help():
		print(mgdiff.helpMSG) 

	def ver():
		print(mgdiff.versionInfo)



	def __param_check_set(self , kwd):

		for k,v in kwd.items():
			if not k in mgdiff.paramter	:
				raise( Exception("KeyError: {} in {} ".format(k,self.__class__.__name__) ) )

		self.msgoff = True

		# edgeファイル名
		if not "ei" in kwd :
			raise Exception("ei= is necessary")
		self.ei = kwd["ei"] 
		
		# edgeファイル名
		if not "eI" in kwd :
			raise Exception("eI= is necessary")
		self.eI = kwd["eI"] 
		

		self.ni = kwd["ni"] if "ni" in kwd else None # node file name
		self.nI = kwd["nI"] if "nI" in kwd else None # node file name

		if (self.ni and self.nI==None) or (self.ni == None and self.nI) :
			raise("must specify both of ni= and nI=")


		# edge出力ファイル名
		if not "eo" in kwd :
			raise Exception("eo= is necessary")
		self.eo = kwd["eo"] 

		# Node出力ファイル名
		self.no = kwd["no"] if "no" in kwd else None # node file name

		# ---- edge field names (two nodes) on ei=
		if "ef" in kwd:
			ef0 = kwd["ef"].split(",")
			self.ef1 = ef0[0]
			self.ef2 = ef0[1] 
		else:
			self.ef1 = "node1"
			self.ef2 = "node2"

		if "eF" in kwd:
			ef0 = kwd["eF"].split(",")
			self.eF1 = ef0[0]
			self.eF2 = ef0[1] 
		else:
			self.eF1 = self.ef1 
			self.eF2 = self.ef2 

		# ---- node field name on ni=
		self.nf = kwd["nf"] if "nf"  in kwd else "node"

		self.nF = kwd["nF"] if "nF"  in kwd else "node"

		if not "eF" in kwd: #なぜ？
			self.nF = self.nf

		self.dir = kwd["dir"] if "dir" in kwd else False

		#ワークファイルパス
		if "T" in kwd :
			os.environ["KG_TmpPath"] =  re.sub('\/$','',kwd["T"])

	def __cmdline(self):
		cmdline = self.__class__.__name__
		for k,v in self.args.items():
			if type(v) is bool :
				if v == True :
					cmdline += " -" + str(k)
			else:
				cmdline += " " + str(k) + "=" + str(v)
		return cmdline 

	def __init__(self,**kwd):

		#パラメータチェック
		self.args = kwd
		self.__param_check_set(kwd)

	# ============
	# entry point
	def run(self,**kw_args):
	
		os.environ['KG_ScpVerboseLevel'] = "2"
		if "msg" in kw_args:
			if kw_args["msg"] == "on":
				os.environ['KG_ScpVerboseLevel'] = "4"

		from datetime import datetime	
		t = datetime.now()

		wf = nu.Mtemp()
		xxedge_i = wf.file()
		xxedge_I = wf.file()

		# クリーニング(ei=)
		fei = None
		fei <<= nm.mcut(f=[self.ef1,self.ef2], i=self.ei)
		if not self.dir :
			fei <<= nm.mfsort(f=[self.ef1,self.ef2])
		fei <<= nm.muniq(k=[self.ef1,self.ef2])
		fei <<= nm.msetstr(a="ei",v=self.ei)
		
		# クリーニング(ei=)
		feI = None
		feI <<= nm.mcut(f="%s:%s,%s:%s"%(self.eF1,self.ef1,self.eF2,self.ef2), i=self.ei)
		if not self.dir :
			feI <<= nm.mfsort(f=[self.ef1,self.ef2])
		feI <<= nm.muniq(k=[self.ef1,self.ef2])
		feI <<= nm.msetstr(a="eI",v=self.eI)

		fe = None
		fe <<= nm.mjoin(k=[self.ef1,self.ef2], m=feI,i=fei,f="eI",n=True,N=True)
		fe <<= nm.mcal(c='if(isnull($s{ei}),-1,if(isnull($s{eI}),1,0))',a="diff")
		fe <<= nm.mcut(f=[self.ef1,self.ef2,"ei","eI","diff"],o=self.eo)

		fe.run()

		if self.ni and self.nI :
			# クリーニング(ni=)
			fni =   nm.mcut(f=self.nf,i=self.ni)
			fni <<= nm.muniq(k=self.nf)
			fni <<= nm.msetstr(a="ni",v=self.ni)

			fnI =   nm.mcut(f="%s:%s"%(self.nF,self.nf),i=self.nI)
			fnI <<= nm.muniq(k=self.nf)
			fnI <<= nm.msetstr(a="nI",v=self.nI)
			
			fn = None
			fn <<= nm.mjoin(k=self.nf,m=fnI,i=fni,f="nI",n=True,N=True)
			fn <<= nm.mcal(c='if(isnull($s{ni}),-1,if(isnull($s{nI}),1,0))',a="diff")
			fn <<= nm.mcut(f=[self.nf,"ni","nI","diff"],o=self.no)

			fn.run()



		nu.mmsg.endLog(self.__cmdline())



