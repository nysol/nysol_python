#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import os.path
import shutil

import nysol.mcmd as nm
import nysol.util.mtemp as mtemp
from nysol.take import graph as ntg
from nysol.take import extcore as extTake


class mpolishing(object):

	helpMSG="""
----------------------------
#{$cmd} version #{$version}
----------------------------
概要) グラフデータの研磨をおこなう。
内容) 一般グラフデータを入力として、密度の高い部分グラフにあって枝が張られていないノードペアに枝を張る。
      逆に、密度の低い部分グラフにあって枝が張られているノードペアの枝を刈る。
      新たに張られる枝や刈られる枝の程度は、sim=とth=で与えた値によって変わる。
      詳細は以下に示したアルゴリズムを参照。
書式) #{$cmd} ei= [ef=] [ni=] [nf=] eo= [no=] [sim=i|I|s|S|T|R|P|C] th= [sup=] [-indirect] [iter=] [log=] [T=] [--help]

  ファイル名指定
  ei=    : 枝データファイル
  ef=    : 枝データ上の2つの節点項目名(省略時は"node1,node2")
  ni=    : 節点データファイル
  nf=    : 節点データ上の節点項目名(省略時は"node")
  eo=    : データ研磨後の枝データファイル
  no=    : データ研磨後の節点データファイル
  sim=   : 節点a,bと接続された枝集合を、それぞれA,Bとすると、節点a,bに枝を張るために用いる類似度。
           省略時はRが設定される。
             i: inclusion
             I: both-inclusion
             S: |A∩B|/max(|A|,|B|)
             s: |A∩B|/min(|A|,|B|)
             T (intersection): find pairs having common [threshld] items
             R (resemblance): find pairs s.t. |A\capB|/|A\cupB| >= [threshld]
             P (PMI): find pairs s.t. log (|A\capB|*|all| / (|A|*|B|)) >= [threshld]
             C (cosine distance): find pairs s.t. inner product of their normalized vectors >= [threshld]
  th=    : sim=で指定された類似度について、ここで指定された値以上の節点間に枝を張る。
  sup=   : 類似度計算において、|A∩B|>=supの条件を加える。省略すればsup=0。
  -indirect: 上記類似度計算における隣接節点集合から直接の関係を除外する。
             すなわち、A=A-b, B=B-a として類似度を計算する。
  iter=  : データ研磨の最大繰り返し数(デフォルト=30)
  log=   : パラメータの設定値や収束回数等をkey-value形式のCSVで保存するファイル名

  その他
  T= : ワークディレクトリ(default:/tmp)
  O=     : デバッグモード時、データ研磨過程のグラフを保存するディレクトリ名
  --help : ヘルプの表示

備考)
内部で起動しているコマンドsspcは0から始まる整数で指定された節点名を前提として処理する。
一方で本コマンドは、任意の文字列で節点名を表したデータを処理できる。
それは、sspcを実行する前に、それら文字列と整数との対応表を前処理で作成しているからである。

アルゴリズム)
  # 本コマンドでデフォルトで設定されている類似度関数。
  function sim(E,a,b,th,minSup)
    A=E上での節点aの隣接節点集合
    B=E上での節点bの隣接節点集合
		if |A ∩ B|>=minSup and |A ∩ B|/|A ∪ B|>=th
			return TRUE
		else
			return FALSE
		end
  end

  function conv(E,sim,th,minSup)
    foreach(e∈E)
      a,b=eを構成する2つの節点
      if sim(E,a,b,th,minSup)
        E' = E' ∪ edge(a,b)
      end
    end
    return E'
  end

  polishing(E,sim,th,minSup,iter)
    E:=グラフを構成する枝集合
    iter:=最大繰り返し回数
    sim:=類似度関数
    th:=最小類似度
    c=0
    while(c<iter)
      E'=conv(E,sim,th,minSup)
      if E==E'
        break
      end
      E=E'
      c=c+1
    end
    return E
  end

例) 枝データからのみグラフ研磨を実行する例
$ cat edge.csv
n1,n2
a,b
a,d
b,c
b,d
c,d
d,e
$ #{$cmd} ei=edge.csv ef=n1,n2 th=0.5 eo=output.csv
#MSG# converting graph files into a pair of numbered nodes ...; 2013/10/10 14:48:03
#MSG# polishing iteration #0; 2013/10/10 14:48:04
#MSG# converting the numbered nodes into original name ...; 2013/10/10 14:48:04
#END# #{$cmd} ei=edge.csv ef=n1,n2 th=0.1 eo=output.csv; 2013/10/10 14:48:04
$ cat output.csv
n1,n2
a,b
a,c
a,d
b,c
b,d
c,d

# Copyright(c) NYSOL 2018- All Rights Reserved.
		"""

	verInfo="version=1.2"

	def help():
		print(mpolishing.helpMSG) 

	def ver():
		print(mpolishing.versionInfo)

	def __init__(self,gi=None,go=None,sim="R",th=None,indirect=False,sup=0,iter=30,O=None,log=None):

		self.gi				= None			# (graph object) input graph set
		self.go				= None			# (graph object) output graph set
		self.__eo			= None			# (string) output edge file name
		self.__no			= None			# (string) output node file name
		self.__nf			= None			# (string) output node file column title
		self.th				= float(th)	# (float) threshhold of degree of similarity
		self.indirect	= indirect	# (boolean) exclude direct relationship from adjacent node set in similarity calculation
		self.measure	= sim				# (string) similarity measure
		self.minSupp	= int(sup) # (float) minimum support
		self.iterMax	= int(iter)	    # (int) upper bound of iterations
		self.logFile	= log				# (string) log file name
		self.outDir		= O					# (string) directory name for outputs in the process
		self.msgoff		= True
		self.__tempW	= mtemp.Mtemp()	# temp files created by this instance will be deleted, after self.run() executed.

		if(gi):
			self.gi = gi						# input graph object
		else:
			raise Exception("gi is mandatory.")
		
		if(go):
			self.go = go
		self.__eo = self.__tempW.file() 
		self.__no = self.__tempW.file()
		self.__nf = "n"
		
		if self.iterMax < 1:
			raise Exception("iter must be 1 or more.")

		if self.outDir and not os.path.isdir(self.outDir) :
			os.makedirs(self.outDir)

	# count node size and edge size for sspc's output file
	def calGsize(self,file):
		nodes = set([])
		edgeSize=0
		with open(file) as f:
			for line in f:
				line = line.rstrip('\r\n')
				n1,n2=line.split(" ")
				nodes.add(n1)
				nodes.add(n2)
				edgeSize+=1

		return len(nodes),edgeSize

	# calculate graph features of sspc's output file
	def features(self,file,orgNsize=None):
		nodeSize,edgeSize=self.calGsize(file)
		nSize=float(nodeSize)
		if(orgNsize):
			nSize=float(orgNsize)
		dens=None
		if(nSize>1):
			dens=float(edgeSize)/(nSize*(nSize-1)/2.0) 
		return nSize,edgeSize,dens

	# compare sspc' output files before and after
	def same(self,file1,file2):
		xx = self.__tempW.file()
		if os.path.getsize(file1)!= os.path.getsize(file2):
			return False
		os.system("diff -q %s %s > %s"%(file1,file2,xx))
		if(os.path.getsize(xx) != 0):
			return False
		return True


	# execute
	def run(self):

		from datetime import datetime	
		t = datetime.now()

		input=self.gi.eFileT
		xxmap = self.gi.mFile
		xxmaprev = self.__tempW.file()
		nm.msortf(f="id",i=xxmap,o=xxmaprev).run()


		xxpair = self.__tempW.file() # sscpの出力(pair形式)
		xxtra  = self.__tempW.file() # sscpの入力(tra形式)
		xxprev = self.__tempW.file() # 前回のxxtra
		xxtmmp = self.__tempW.file()

		shutil.copyfile(input,xxpair)

		nSizes=[]
		eSizes=[]
		denses=[]

		iter=0
		while True :
			# グラフ特徴量の計算
			if(self.logFile):
				nSize,eSize,dens=self.features(xxpair)
				nSizes.append(nSize)
				eSizes.append(eSize)
				denses.append(dens)

			# node pairをsspc入力形式に変換

			if self.indirect :
				gtpstri = "ue_" if self.msgoff else "ue"
				extTake.grhfil(type=gtpstri,i=xxpair,o=xxtra)
			else:
				gtpstri0 = "ue0_" if self.msgoff else "ue0"
				extTake.grhfil(type=gtpstri0,i=xxpair,o=xxtra)
				
			para = "%s,%s"%(self.gi.edgeFN1,self.gi.edgeFN2)

			if self.outDir :
				os.system("tr ' ' ',' < %s > %s "%(xxpair,xxtmmp))
				f = nm.mcut(f="0:num1,1:num2",nfni=True,i=xxtmmp)
				f <<= nm.mjoin(k="num1",K="id",m=xxmaprev,f="node:%s"%(self.gi.edgeFN1))
				f <<= nm.mjoin(k="num2",K="id",m=xxmaprev,f="node:%s"%(self.gi.edgeFN2)).mcut(f=para).mfsort(f=para)
				f <<= nm.msortf(f=para,o="%s/pair_%s.csv"%(self.outDir,iter))
				f.run()
				
			# 終了判定
			if(iter>=self.iterMax):
				break

			if iter!=0 and self.same(xxtra,xxprev):
				break

			shutil.copyfile(xxtra,xxprev)

			tpstr = self.measure+"_"		if self.msgoff else self.measure

			extTake.sspc(type=tpstr,l=self.minSupp,i=xxtra,th=self.th,o=xxpair)


			#gtpstr = "ue0_" if self.msgoff else "ue0"
			#extTake.grhfil(type=gtpstr,i=xxpair,o=xxtra)

			iter+=1


		# 上記iterationで収束したマイクロクラスタグラフを元の節点文字列に直して出力する
		os.system("tr ' ' ',' < %s > %s"%(xxpair,xxtmmp))
		f = nm.mcut(f="0:num1,1:num2",nfni=True,i=xxtmmp)
		f <<= nm.mjoin(k="num1",K="id",m=xxmaprev,f="node:%s"%(self.gi.edgeFN1)).mjoin(k="num2",K="id",m=xxmaprev,f="node:%s"%(self.gi.edgeFN2))
		f <<= nm.mcut(f=para).mfsort(f=para).msortf(f=para,o=self.__eo)
		f.run()

		if(self.__no):
			if self.__nf:
				nm.mcut(f="node:%s"%(self.__nf),i=xxmap,o=self.__no).run()
			else:
				nm.mcut(f="node",i=xxmap,o=self.__no).run()

		if(self.go):
			self.go.readCSV(edgeFile=self.__eo,title1=self.gi.edgeFN1,title2=self.gi.edgeFN2,nodeFile=self.__no,title=self.__nf)
		else:
			self.go = ntg.graph(edgeFile=self.__eo,title1=self.gi.edgeFN1,title2=self.gi.edgeFN2,nodeFile=self.__no,title=self.__nf)

		procTime = datetime.now()-t
		# ログファイル出力
		if(self.logFile):
			kv=[["key","value"]]
			
			kv.append(["iter",str(self.iterMax)])
			kv.append(["outDir",str(self.outDir)])
			kv.append(["th",str(self.th)])
			kv.append(["indirect",str(self.indirect)])
			kv.append(["measure",str(self.measure)])

			kv.append(["minSupp",str(self.minSupp)])
			kv.append(["logFile",str(self.logFile)])
			kv.append(["outDir",str(self.outDir)])


			kv.append(["time",str(procTime)])
			for i in range(len(nSizes)):
				kv.append(["nSize"+str(i),str(nSizes[i])]);
				kv.append(["eSize"+str(i),str(eSizes[i])]);
				kv.append(["dens"+str(i) ,str(denses[i])]);

			nm.writecsv(i=kv,o=self.logFile).run()

		return self.go
	 
