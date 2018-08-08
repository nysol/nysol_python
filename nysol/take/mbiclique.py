#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import shutil

import nysol.mcmd as nm
import nysol.util.margs as margs
import nysol.util.mtemp as mtemp
from nysol.take import extcore as extTake


class mbiclique(object):

	helpMSG="""
----------------------------
mbiclique.rb version #{$version}
----------------------------
概要) lcmによる極大二部クリークの列挙
内容) 二部グラフデータを入力として、極大二部クリークを列挙する。
書式) mbiclique.rb ei= [ef=] [o=] [l=] [u=] [o=] [-edge] [T=] [-debug] [--help]

  ファイル名指定
  ei=    : 辺データファイル
  ef=    : 辺データ上の2つの部項目名(省略時は"node1,node2")
  o=     : 出力ファイル
  l=     : 二部クリークを構成する最小節点数(ここで指定したサイズより小さいクリークは列挙されない)
         : カンマで区切って2つの値を指定すると、各部のサイズを制限できる
         : 1つ目の値はef=で指定した1つ目の部に対応し、2つ目の値は2つ目に指定した部に対応する。
  u=     : クリークを構成する最大節点数(ここで指定したサイズより大きいクリークは列挙されない)
         : カンマで区切って2つの値を指定すると、各部のサイズを制限できる
	-edge  : 枝による出力(クリークIDと枝(節点ペア)で出力する)

  その他
  T= : ワークディレクトリ(default:/tmp)
  --help : ヘルプの表示

入力形式)
二部グラフの節点ペアを項目で表現したCSVデータ。

出力形式1)
二部クリークを構成する全節点を各部ごとにベクトル形式で出力する。
出力項目は、"節点項目名1,節点項目名2,size1,size2"の4項目で、節点名1と節点名2は、ef=で指定された名称が利用される。
節点項目名1,節点項目名2に出力される値が節点名ベクトルである(一行が一つの二部クリークに対応)ことが異なる。
idはクリークの識別番号で、一つのクリークは同じid番号が振られる。id番号そのものに意味はない。
節点項目名1,節点項目名2には、各部を構成する節点名のベクトルが出力される。
size1,size2は二部クリークを構成する各部の節点数である。

出力形式2) -edge を指定した場合の出力形式
クリークIDと二部クリークを構成する全枝(節点ペア)を出力する。
出力項目は"id,節点項目名1,節点項目名2,size"の4項目である。
例えば各部のサイズが3,4であるような二部クリークは12行の枝データとして出力される。
出力形式1に比べてファイルサイズは大きくなる。


例1)
$ cat data1.csv
node1,node2
a,A
a,B
a,C
b,A
b,B
b,D
c,A
c,D
d,B
d,C
d,D

$ mclique.rb ei=data1.csv ef=n1,n2 o=out1.csv
#MSG# converting paired form into transaction form ...; 2014/03/24 11:52:05
#MSG# lcm_20140215 CIf /tmp/__MTEMP_47150_70177387663280_0 1 /tmp/__MTEMP_47150_70177387663280_3; 2014/03/24 11:52:05
trsact: /tmp/__MTEMP_47150_70177387663280_0 ,#transactions 4 ,#items 4 ,size 11 extracted database: #transactions 4 ,#items 4 ,size 11
output to: /tmp/__MTEMP_47150_70177387663280_3
separated at 0
11
1
3
4
3
iters=11
#END# mbiclique.rb ei=data1.csv o=out1.csv ef=node1,node2
$ cat out1.csv 
node1,node2,size1,size2
a,A B C,1,3
a b,A B,2,2
a b c,A,3,1
a b d,B,3,1
a d,B C,2,2
b,A B D,1,3
b c,A D,2,2
b c d,D,3,1
b d,B D,2,2
d,B C D,1,3

例3) 枝による出力(-edgeの指定)
$ mclique.rb ei=data1.csv ef=n1,n2 o=out2.csv
#END# ../../bin/mbiclique.rb ei=data/data1.csv o=xxresult/out11.csv ef=node1,node2 -edge
$ cat out2.csv 
id,node1,node2,size1,size2
1,c,A,3,1
1,a,A,3,1
1,b,A,3,1
10,d,B,1,3
10,d,C,1,3
10,d,D,1,3
2,b,B,3,1
2,a,B,3,1
2,d,B,3,1
   :

例3) 部node1の最小サイズを3に制限
$ mbiclique.rb ei=data1.csv o=out3.csv ef=node1,node2 l=3,
#END# mbiclique.rb ei=data1.csv o=out3.csv ef=node1,node2 l=3,
$ cat out3.csv 
node1,node2,size1,size2
a b c,A,3,1
a b d,B,3,1
b c d,D,3,1

# Copyright(c) NYSOL 2012- All Rights Reserved.
"""

	verInfo="version=1.2"

	paramter = {	
		"ei":"filename",
		"ef":"fld",
		"l":"int",
		"u":"int",
		"o":"filename",		
		"edge":"bool",
		"T":"str"
	}

	paramcond = {	
		"hissu": ["ei"]
	}	


	def help():
		print(mbiclique.helpMSG) 

	def ver():
		print(mbiclique.verInfo)

	def __param_check_set(self , kwd):

		for k,v in kwd.items():
			if not k in mbiclique.paramter	:
				raise( Exception("KeyError: {} in {} ".format(k,self.__class__.__name__) ) )
			#型チェック入れる

		self.msgoff = True

		self.byedge  = kwd["edge"] if "edge" in kwd else False 
		self.oFile   = kwd["o"]   if "o"   in kwd else None

		# ---- edge field names (two nodes) on ei=
		self.ei = kwd["ei"] # edgeファイル名
		ef0 = kwd["ef"].split(",")
		self.ef1 = ef0[0]
		self.ef2 = ef0[1] 

		minSize = kwd["l"].split(",") if "l"   in kwd else None   # クリークサイズ下限
		maxSize = kwd["u"].split(",") if "u"   in kwd else None   # クリークサイズ上限

		self.minSize1 = ""
		self.minSize2 = ""
		self.maxSize1 = ""
		self.maxSize2 = ""


		if minSize != None :

			if len(minSize) == 1:
				if minSize[0] != "":
					self.minSize1 = minSize[0]
					self.minSize2 = minSize[0]
			else:
				if minSize[0] != "":
					self.minSize1 = minSize[0]
				if minSize[1] != "":
					self.minSize2 = minSize[1]

		if maxSize != None :

			if len(maxSize) == 1:
				if maxSize[0] != "":
					self.maxSize1 = maxSize[0]
					self.maxSize2 = maxSize[0]
			else:
				if maxSize[0] != "":
					self.maxSize1 = maxSize[0]
				if maxSize[1] != "":
					self.maxSize2 = maxSize[1]

	def __init__(self,**kwd):
		#パラメータチェック
		self.args = kwd
		self.__param_check_set(kwd)



	def pair2tra(self,ei,ef1,ef2,traFile,mapFile1,mapFile2):

		f1 = None
		f1 <<= nm.mcut(f="{}:node1".format(self.ef1),i=self.ei)
		f1 <<= nm.mdelnull(f="node1")
		f1 <<= nm.muniq(k="node1")
		f1 <<= nm.mnumber(s="node1", a="num1",o=mapFile1)

		f2 = None
		f2 <<= nm.mcut(f="{}:node2".format(self.ef2),i=self.ei)
		f2 <<= nm.mdelnull(f="node2")
		f2 <<= nm.muniq(k="node2")
		f2 <<= nm.mnumber(s="node2",a="num2",o=mapFile2)

		f3 = None
		f3 <<= nm.mcut(f="{}:node1,{}:node2".format(self.ef1,self.ef2),i=self.ei)
		f3 <<= nm.mjoin(k="node1",m=f1,f="num1")
		f3 <<= nm.mjoin(k="node2",m=f2,f="num2")
		f3 <<= nm.mcut(f="num1,num2")
		f3 <<= nm.msortf(f="num1,num2%n")
		f3 <<= nm.mtra(k="num1",s="num2%n",f="num2")
		f3 <<= nm.msortf(f="num1%n")
		f3 <<= nm.mcut(f="num2",nfno=True,o=traFile)
		f3.run()
		os.system("cat "+traFile)

	# ============
	# entry point
	def run(self):

		tempW	= mtemp.Mtemp()

		xxtra = tempW.file()
		xxmap1 = tempW.file()
		xxmap2 = tempW.file()
		lcmout = tempW.file()

		xxt0 = tempW.file()
		xxp0 = tempW.file()
		xx3t = tempW.file()
		xx4t = tempW.file()

		self.pair2tra(self.ei,self.ef1,self.ef2,xxtra,xxmap1,xxmap2)

		runPara={}
		runPara["type"] = "CIf"
		runPara["sup"] = 1
		runPara["o"] = lcmout
		runPara["i"] = xxtra

		if self.minSize2:
			runPara["l"] = self.minSize2
		if self.maxSize2:
			runPara["u"] = self.maxSize2


		extTake.lcm(runPara)
		extTake.lcmtrans(lcmout,"p",xxt0)

		f = None
		f <<= nm.mdelnull(f="pattern",i=xxt0)
		f <<= nm.mvreplace(vf="pattern",m=xxmap2,K="num2",f="node2")
		f <<= nm.mcut(f="pid,pattern,size:size2")
		f <<= nm.mvsort(vf="pattern")
		f <<= nm.msortf(f="pid")
		
		if self.byedge :
			f_e0 = nm.mtra(f="pattern",i=f,r=True)
			extTake.lcmtrans(lcmout,"t",xx3t)

			f_e1 = None
			f_e1 <<= nm.mjoin(k="__tid",m=xxmap1,f="node1",K="num1",i=xx3t)
			f_e1 <<= nm.msortf(f="pid")
			## xx2
			f_e2 = None
			f_e2 <<= nm.mcount(k="pid",a="size1",i=f_e1)
			f_e2 <<= nm.mselnum(f="size1",c="[{},{}]".format(self.minSize1,self.maxSize1))
			
			f_e3 = None
			f_e3 <<= nm.mjoin(k="pid",m=f_e2,f="size1",i=f_e1)
			f_e3 <<= nm.mnjoin(k="pid",m=f_e0,f="pattern,size2")
			f_e3 <<= nm.mcut(f="pid:id,node1:{},pattern:{},size1,size2".format(self.ef1,self.ef2),o=self.oFile)
			f_e3.run()

		else:

			extTake.lcmtrans(lcmout,"t",xx4t)
			f_e4 = None
			f_e4 <<= nm.mjoin(k="__tid",m=xxmap1,i=xx4t,f="node1",K="num1")
			f_e4 <<= nm.mtra(k="pid",f="node1")
			f_e4 <<= nm.mvcount(vf="node1:size1")
			f_e4 <<= nm.mjoin(k="pid",m=f,f="pattern,size2")
			f_e4 <<= nm.mselnum(f="size1",c="[{},{}]".format(self.minSize1,self.maxSize1))
			f_e4 <<= nm.mvsort(vf="node1,pattern")
			f_e4 <<= nm.msortf(f="node1,pattern")
			f_e4 <<= nm.mcut(f="node1:{},pattern:{},size1,size2".format(self.ef1,self.ef2),o=self.oFile)
			f_e4.run()

