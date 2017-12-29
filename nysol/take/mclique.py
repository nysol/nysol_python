#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import os.path
import shutil

import nysol.mod as nm
import nysol.util.margs as margs
import nysol.util.mtemp as mtemp
import nysol.util.mrecount as mrecount
import nysol.take.extcore as extTake

class mclique(object):
	def help(self):
		msg = """

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

	def ver(self):
		print("version #{$version}")

	def __init__(self,args):
		self.args = args
		self.msgoff = True

		# mcmdのメッセージは警告とエラーのみ
		#ENV["KG_VerboseLevel"]="2" unless args.bool("-mcmdenv")

		#ワークファイルパス
		#if args.str("T=")!=nil then
		#	ENV["KG_TmpPath"] = args.str("T=").sub(/\/$/,"")
		#end

		self.all=args.bool("-all")

		self.ei = args. file("ei=","r") # edgeファイル名
		self.ni = args. file("ni=","r") # node file name

		# ---- edge field names (two nodes) on ei=
		self.ef1,self.ef2 = args.field("ef=", self.ei, "node1,node2",2,2)["names"]

		# ---- node field name on ni=
		self.nf = args.field("nf=", self.ni, "node",1,1)
		if self.nf:
			self.nf = self.nf["names"][0]

		self.minSize = args.int("l=")    # クリークサイズ下限
		self.maxSize = args.int("u=")    # クリークサイズ上限
		self.oFile   = args.file("o=", "w")
		self.logFile = args.file("log=", "w")


	def g2pair(self,ni,nf,ei,ef1,ef2,ipair,mapFile):
	#MCMD::msgLog("converting graph files into a pair of numbered nodes ...")
		xxt = mtemp.Mtemp()
		wf1 = xxt.file() 
		wf2 = xxt.file() 
		wf3 = xxt.file() 

		nm.mcut(f=str(ef1)+":node",i=ei,o=wf1).run()
		nm.mcut(f=str(ef2)+":node",i=ei,o=wf2).run()
		if(ni): 
			nm.mcut(f=str(nf)+":node",i=ni,o=wf3).run()
			
		if(ni): 
			para="%s,%s,%s"%(wf1,wf2,wf3)
		else:
			para="%s,%s"%(wf1,wf2)
		nm.mcat(i=para,f="node").muniq(k="node").mnumber(q=True,a="num",o=mapFile).run()


		paraE="%s,%s"%(ef1,ef2)
		f =   nm.mcut(f=paraE,i=ei)
		f <<= nm.mjoin(k=ef1,K="node",m=mapFile,f="num:num1")
		f <<= nm.mjoin(k=ef2,K="node",m=mapFile,f="num:num2")
		f <<= nm.mcut(f="num1,num2")
		f <<= nm.mfsort(f="num1,num2")
		f <<= nm.msortf(f="num1%n,num2%n" , nfno=True,o=ipair)
		f.run()

	# ============
	# entry point
	def run(self):
		from datetime import datetime	
		t = datetime.now()


		wf = mtemp.Mtemp()
		xxinp    = wf.file()
		xxmap    = wf.file()
		input    = self.ei

		self.g2pair(self.ni,self.nf,self.ei,self.ef1,self.ef2,xxinp,xxmap)


		input=xxinp

		xxmace = wf.file() # maceの出力(tra形式)
		xxpair = wf.file() # 上記traをpair形式に変換したデータ
		para = {}
		if self.msgoff :
			para["type"] = "Ce_" if self.all else "Me_"
		else:
			para["type"] = "Ce" if self.all else "Me"
			
		para["i"] = input
		para["o"] = xxmace

		if self.minSize :
			para["l"] = self.minSize

		if self.maxSize :
			para["u"] = self.maxSize

		extTake.mace(para)
		
		#MCMD::msgLog("converting the numbered nodes into original name ...")
		id= mrecount.mrecount(i=xxmace,nfni=True)
		fld="id,num,size"
		f=None
		f <<= nm.mcut(i=xxmace,nfni=True,f="0:num")
		f <<= nm.mnumber(q=True,a="id")
		f <<= nm.mvcount(vf="num:size")
		f <<= nm.mtra(r=True,f="num",o=xxpair)
		f.run()
		

		# when ni= specified, it add the isolated single cliques.
		if self.ni :
			xxiso =wf.file()
			xxcat =wf.file()

			f0=None
			f0 <<= nm.msortf(f="num",i=xxpair)
			if self.all:
				f0 <<= nm.mselstr(f="size",v=1)
			f0 <<= nm.mcut(f="num")
			f0 <<= nm.muniq(k="num")


			# select all nodes which are not included in any cliques
			f=None
			f <<= nm.mcut(f="num",i=xxmap)
			f <<= nm.mcommon(k="num",m=f0,r=True)
			f <<= nm.mnumber(S=id,a="id",q=True)
			f <<= nm.msetstr(v=1,a="size")
			f <<= nm.mcut(f="id,num,size",o=xxiso)
			f.run()

			nm.mcat(i=xxpair+","+xxiso,o=xxcat).run()
			shutil.copyfile(xxcat,xxpair)


		f = nm.mjoin(m=xxmap,i=xxpair,k="num",f="node")
		f <<= nm.mcut(f="id,node,size")
		f <<= nm.msortf(f="id,node",o=self.oFile)
		f.run()

		procTime = datetime.now()-t

		# ログファイル出力
		if (self.logFile) :
			kv=[["key","value"]]
			kv.extend(self.args.getKeyValue())
			kv.append(["time",str(procTime)])
			nm.writecsv(i=kv,o=self.logFile).run()

		# 終了メッセージ
		#MCMD::endLog(args.cmdline)


if __name__ == '__main__':
	import sys
	args=margs.Margs(sys.argv,"ei=,ef=,ni=,nf=,-all,o=,l=,u=,log=","ei=,ef=")
	mclique(args).run()
	#"ei=,ef=,th="


