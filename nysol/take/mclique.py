#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import os.path
import shutil

import nysol.mcmd as nm
import nysol.util as nu

#import nysol.util.margs as margs
#import nysol.util.mtemp as mtemp
#import nysol.util.mrecount as mrecount

from nysol.take import extcore as extTake

class mclique(object):

	helpMSG="""
# ver="1.0" # 初期リリース 2014/2/20
# ver="1.1" # eo=,no=の機能追加 2014/3/27
# ver="1.2" # ni=,nf=の追加, eo=,no=の機能をmclique2g.rbに分離, 枝出力を廃止(-nodeによる出力のみ) 2014/8/2
----------------------------
#{$cmd} version #{$version}
----------------------------
概要) 一般グラフデータから極大クリークを列挙する。
書式) #{$cmd} ei= ef= [ni=] [nf=] [o=] [l=] [u=] [-node] [-all] [log=] [T=] [--help]

  ファイル名指定
  ei=    : 枝データファイル
  ef=    : 枝データ上の2つの節点項目名(省略時は"node1,node2")
  ni=    : 節点データファイル
  nf=    : 節点データ上の節点項目名(省略時は"node")
  o=     : 出力ファイル(クリークID-枝:-nodeを指定することでクリークID-節点に変更可能,省略時は標準出力)
  l=     : クリークを構成する最小節点数(ここで指定したサイズより小さいクリークは列挙されない)
  u=     : クリークを構成する最大節点数(ここで指定したサイズより大きいクリークは列挙されない)
  -all   : 極大クリークだけでなく、全クリークを列挙する。
  log=   : パラメータの設定値をkey-value形式のCSVで保存するファイル名

  その他
  T= : ワークディレクトリ(default:/tmp)
  --help : ヘルプの表示

入力形式)
一般グラフを節点ペアで表現した形式。
他のいかなる節点とも接続のない節点は、サイズが１の自明なクリークであるため、入力対象外とする。

o=の出力形式)
クリークIDと接点を出力する。
出力項目は"id,node,size"の3項目である。
sizeはクリークを構成する節点数である。

備考)
内部で起動しているコマンドmaceは0から始まる整数で指定された節点名を前提として処理する。
一方で本コマンドは、任意の文字列で節点名を表したデータを処理できる。
それは、maceを実行する前に、それら文字列と整数との対応表を前処理で作成しているからである。

例)
$ cat data/edge.csv 
n1,n2
a,b
a,c
a,d
b,c
b,d
b,e
c,d
d,e

$ #{$cmd} ei=edge.csv ef=n1,n2 o=output.csv
#MSG# converting graph files into a pair of numbered nodes ...; 2014/01/06 14:27:17
#MSG# converting the numbered nodes into original name ...; 2014/01/06 14:27:17
#END# #{$cmd} ei=data/edge1.csv ef=n1,n2 -node o=output.csv; 2014/01/06 14:27:17
$ cat output.csv 
id,node,size
0,b,3
0,d,3
0,e,3
1,a,4
1,b,4
1,c,4
1,d,4

例) 節点ファイルも指定した例
$ cat edge.csv 
n1,n2
c,a
c,d
d,e
a,d
a,e

$ cat node.csv 
n
a
b
c
d
e
f

$ #{$cmd} ei=edge.csv ef=n1,n2 ni=node.csv nf=n o=output.csv
#MSG# converting graph files into a pair of numbered nodes ...; 2014/01/06 14:27:17
#MSG# converting the numbered nodes into original name ...; 2014/01/06 14:27:17
#END# #{$cmd} ei=data/edge1.csv ef=n1,n2 -node o=output.csv; 2014/01/06 14:27:17
$ cat output.csv 
id,node,size
0,b,1
1,a,3
1,d,3
1,e,3
2,a,3
2,c,3
2,d,3
3,f,1

# Copyright(c) NYSOL 2012- All Rights Reserved.
		"""

	verInfo="version=1.2"

	paramter = {	
		"ei":"filename",
		"ef":"fld",
		"ni":"filename",
		"nf":"fld",
		"l":"int",
		"u":"int",
		"o":"filename",		
		"log":"filename",
		"all":"bool",
		"T":"str"
	}

	paramcond = {	
		"hissu": ["ei","ef"]
	}

	def help():
		print(mclique.helpMSG) 

	def ver():
		print(mclique.versionInfo)



	def __param_check_set(self , kwd):

		for k,v in kwd.items():
			if not k in mclique.paramter	:
				raise( Exception("KeyError: {} in {} ".format(k,self.__class__.__name__) ) )

		self.msgoff = True

		self.ei = kwd["ei"] # edgeファイル名
		self.ni = kwd["ni"] if "ni"  in kwd else None # node file name
		
		# ---- edge field names (two nodes) on ei=
		ef0 = kwd["ef"].split(",")
		self.ef1 = ef0[0]
		self.ef2 = ef0[1] 

		# ---- node field name on ni=
		self.nf = kwd["nf"] if "nf"  in kwd else "node"

		self.minSize = kwd["l"]   if "l"   in kwd else None   # クリークサイズ下限
		self.maxSize = kwd["u"]   if "u"   in kwd else None   # クリークサイズ上限
		self.oFile   = kwd["o"]   if "o"   in kwd else None
		self.logFile = kwd["log"] if "log" in kwd else None

		self.all = kwd["all"] if "all" in kwd else False

	def __init__(self,**kwd):

		#パラメータチェック
		self.args = kwd
		self.__param_check_set(kwd)


	def g2pair(self,ni,nf,ei,ef1,ef2,ipair,mapFile):

		dlist =[nm.mcut(f=ef1+":node",i=ei),nm.mcut(f=ef2+":node",i=ei)]

		if(ni): 
			dlist.append(nm.mcut(f=nf+":node",i=ni))
			
		fmap = None	
		fmap <<= nm.mcut(i=dlist,f="node")
		fmap <<= nm.muniq(k="node")
		fmap <<= nm.mnumber(q=True,a="num",o=mapFile)

		f =   nm.mcut(f=[ef1,ef2],i=ei)
		f <<= nm.mjoin(k=ef1,K="node",m=fmap,f="num:num1")
		f <<= nm.mjoin(k=ef2,K="node",m=fmap,f="num:num2")
		f <<= nm.mcut(f="num1,num2")
		f <<= nm.mfsort(f="num1,num2")
		f <<= nm.msortf(f="num1%n,num2%n" , nfno=True,o=ipair)
		f.run()
	# ============
	# entry point
	def run(self):

		from datetime import datetime	
		t = datetime.now()

		wf = nu.Mtemp()
		xxinp    = wf.file()
		xxmap    = wf.file()
		input    = self.ei

		self.g2pair(self.ni,self.nf,self.ei,self.ef1,self.ef2,xxinp,xxmap)

		xxmace = wf.file() # maceの出力(tra形式)

		para = {}
		if self.msgoff :
			para["type"] = "Ce_" if self.all else "Me_"
		else:
			para["type"] = "Ce" if self.all else "Me"
		para["i"] = xxinp
		para["o"] = xxmace
		if self.minSize :
			para["l"] = self.minSize
		if self.maxSize :
			para["u"] = self.maxSize
		extTake.mace(para)
		
		#MCMD::msgLog("converting the numbered nodes into original name ...")
		id = nu.mrecount(i=xxmace,nfni=True)

		# xxpair = wf.file() # 上記traをpair形式に変換したデータ

		fpair = None
		fpair <<= nm.mcut(i=xxmace,nfni=True,f="0:num")
		fpair <<= nm.mnumber(q=True,a="id")
		fpair <<= nm.mvcount(vf="num:size")
		fpair <<= nm.mtra(r=True,f="num")
		

		# when ni= specified, it add the isolated single cliques.
		if self.ni :

			fpair_u = nm.mread(i=fpair)

			if self.all:
				fpair_u <<= nm.mselstr(f="size",v=1)
			fpair_u <<= nm.mcut(f="num")
			fpair_u <<= nm.muniq(k="num")

			# select all nodes which are not included in any cliques
			xxiso=None
			xxiso <<= nm.mcut(f="num",i=xxmap)
			xxiso <<= nm.mcommon(k="num",m=fpair_u,r=True)
			xxiso <<= nm.mnumber(S=id,a="id",q=True)
			xxiso <<= nm.msetstr(v=1,a="size")
			xxiso <<= nm.mcut(f="id,num,size")
			# mcut入れないとおかしくなるあとで直す
			#ddlist = [fpair.mcut(f="id,num,size"),xxiso]
			xxpair = nm.mcut(i=[fpair,xxiso],f="id,num,size")
			
		else:

			xxpair = fpair


		xxpair <<= nm.mjoin(m=xxmap,k="num",f="node")
		xxpair <<= nm.mcut(f="id,node,size")
		xxpair <<= nm.msortf(f="id,node",o=self.oFile)
		xxpair.run()

		procTime = datetime.now()-t

		# ログファイル出力
		if self.logFile :
			kv=[["key","value"]]
			for k,v in self.args.items():
				kv.append([k,str(v)])
			kv.append(["time",str(procTime)])
			nm.writecsv(i=kv,o=self.logFile).run()



