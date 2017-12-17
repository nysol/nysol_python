#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import os.path
import nysol.take._sspclib as ntsspc
import nysol.take._grhfillib as ntgrhfil
import nysol.mod as nm
import margs as nu
import mtemp as nutil
#require "rubygems"
#require "nysol/mcmd"
#require "nysol/take"
#require "set"

# ver="1.0" # 初期リリース 2014/2/20
# ver="1.1" # 節点データ対応、途中経過出力 2014/8/1
# ver="1.2" # 出力nodeファイルの項目名をnf=の値にする 2016/11/11
#$cmd=$0.sub(/.*\//,"")
#$version="1.2"



class mpolishing(object):
	def help(self):
		msg = """
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

# Copyright(c) NYSOL 2012- All Rights Reserved.
		"""

	def ver(self):
		print("version #{$version}")



	def __init__(self,args):
		self.args = args
		self.indirect=args.bool("-indirect")
		self.ei = args. file("ei=","r") # edge file name
		self.ni = args. file("ni=","r") # node file name

		# ---- edge field names (two nodes) on ei=
		self.ef1,self.ef2 = args.field("ef=", self.ei, "node1,node2",2,2)["names"]
		self.nf = args.field("nf=", self.ni, "node",1,1)

		if self.nf:
			self.nf = nf["names"][0] 

		self.measure = args.str("sim=","R")    # similarity measure
		self.minSupp = args.int("sup=",0)      # mimam support
		self.iterMax = args.int("iter=",30,1)  # upper bound of iterations
		self.th      = args.float("th=")       # threashold for similarity measure

		self.eo      = args.file("eo=", "w")
		self.no      = args.file("no=", "w")
		self.logFile = args.file("log=", "w")
		self.outDir  = args.str("O=")	# 過程出力

		os.system("mkdir -p %s"%(self.outDir))



		# mcmdのメッセージは警告とエラーのみ
		#ENV["KG_VerboseLevel"]="2" unless args.bool("-mcmdenv")

		#ワークファイルパス
		#if args.str("T=")!=nil then
		#	ENV["KG_TmpPath"] = args.str("T=").sub(/\/$/,"")
		#end

#indirect=args.bool("-indirect")

#ei = args. file("ei=","r") # edge file name
#ni = args. file("ni=","r") # node file name

## ---- node field name on ni=
#nf = args.field("nf=", ni, "node",1,1)
#nf = nf["names"][0] if nf

#measure = args.str("sim=","R")    # similarity measure
#minSupp = args.int("sup=",0)      # mimam support
#iterMax = args.int("iter=",30,1)  # upper bound of iterations
#th      = args.float("th=")       # threashold for similarity measure

#eo      = args.file("eo=", "w")
#no      = args.file("no=", "w")
#logFile = args.file("log=", "w")
#outDir  = args.str("O=")	# 過程出力
#MCMD::mkDir(outDir) if outDir

	# node数とedge数をカウント
	def calGsize(self,file):
		nodes = set([])
		edgeSize=0
		for lin in nm.readcsv(file):
			n1,n2=lin[0].split(" ")
			nodes.add(n1)
			nodes.add(n2)
			edgeSize+=1
	
		return len(nodes),edgeSize


# graphの各種特徴量を計算する
# orgNsizeが与えられなければ、node数は枝ファイル(file)から計算する。
# 0,1
# 0,2
# 0,3
# 1,2
# 1,3
	def features(self,file,orgNsize=None):
		nodes = set([])
		graph={}
		edgeSize=0
		for lin in nm.readcsv(file):
			n1 , n2 = lin[0].split(" ") 
			#print(len(lin))
			#n1=lin[0]
			#n2=lin[1]
			if int(n1)>int(n2):
				nt=n1; 
				n1=n2; 
				n2=nt

			if n1 in graph:
				s=graph[n1]
				if n2 not in s:
					s << n2
 
			nodes.add(n1)
			nodes.add(n2)			
			edgeSize+=1

		# 密度
		dens=None
		nSize=float(len(nodes))
		if nSize>1.0 :
			dens=float(edgeSize)/(nSize*(nSize-1.0)/2.0) 

		# clustering coefficient
		#for s in graph:意味ない？
		#	size=s.size

		nodeSize,edgeSize=self.calGsize(file)

		nSize=float(nodeSize)
		if(orgNsize):
			nSize=float(orgNsize)

		dens=None
		if(nSize>1):
			dens=float(edgeSize)/(nSize*(nSize-1)/2.0) 

		return nSize,edgeSize,dens

	def same(self,file1,file2):
		xxt = nutil.Mtemp()
		xx = xxt.file() 
		
		if os.path.getsize(file1)!= os.path.getsize(file2):
			return False
			
		os.system("diff -q %s %s > %s",(file1,file2,xx))

		if(os.path.getsize(xx) != 0):
			return False

		return True

	def g2pair(self,ni,nf,ei,ef1,ef2,ipair,mapFile):
		#MCMD::msgLog("converting graph files into a pair of numbered nodes ...")
		print("converting graph files into a pair of numbered nodes ...")
		wf = nutil.Mtemp()
		wf1=wf.file()
		wf2=wf.file()
		wf3=wf.file()
		
		nm.mcut(f=str(ef1)+":node",i=ei,o=wf1).run()
		nm.mcut(f=str(ef2)+":node",i=ei,o=wf2).run()
		if(ni): 
			nm.mcut(f=str(nf)+":node",i=ni,o=wf3).run()

		if(ni): 
			para="%s,%s,%s"%(wf1,wf2,wf3)
		else:
			para="%s,%s"%(wf1,wf2)
		nm.mcat(i=para,f="node").muniq(k="node").mnumber(q=True,a="num",o=mapFile).run(msg="on")
		print("x1")

		wf4=wf.file()

		paraE="%s,%s"%(ef1,ef2)
		ff0 = nm.mcut(f=paraE,i=ei).mjoin(k=ef1,K="node",m=mapFile,f="num:num1").mjoin(k=ef2,K="node",m=mapFile,f="num:num2")
		ff1 = ff0.mcut(f="num1,num2").mfsort(f="num1,num2").msortf(f="num1%n,num2%n" , nfno=True,o=wf4)
		ff1.run()
		os.system("tr ',' ' ' < " + wf4 + ">"+ipair)


# ============
# entry point
	def run(self):
		from datetime import datetime	
		t = datetime.now()

		wf = nutil.Mtemp()
		xxinp    = wf.file()
		xxmap    = wf.file()
		xxmaprev = wf.file()
		input    = self.ei

		self.g2pair(self.ni,self.nf,self.ei,self.ef1,self.ef2,xxinp,xxmap)

# 入力ファイルをノード番号ペアデータ(input)に変換する。
# csvで指定された場合は、番号-アイテムmapデータも作成(xxmap)。
#wf=MCMD::Mtemp.new
#xxinp=wf.file
#xxmap=wf.file
#xxmaprev=wf.file
#input=ei
#self.g2pair(ni,nf,ei,ef1,ef2,xxinp,xxmap)

		input=xxinp
		#systemm, "msortf f=num i=#{xxmap} o=#{xxmaprev}"

		nm.msortf(f="num",i=xxmap,o=xxmaprev).run()

		xxpair = wf.file() # sscpの出力(pair形式)
		xxtra  = wf.file() # sscpの入力(tra形式)
		xxprev = wf.file() # 前回のxxtra
		xxtmmp = wf.file() # 前回のxxtra

		os.system("cp %s %s"%(input,xxpair))

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
			if(self.indirect):
				#system "#{CMD_grhfil} ue  #{xxpair} #{xxtra}"
				ntgrhfil.grhfil_run(type="ue",i=xxpair,o=xxtra)

			else:
				#system "#{CMD_grhfil} ue0 #{xxpair} #{xxtra}"
				ntgrhfil.grhfil_run(type="ue0",i=xxpair,o=xxtra)
				
			para = "%s,%s"%(self.ef1,self.ef2)
			if(self.outDir):
				os.system("tr ' ' ',' < %s > %s"%(xxpair,xxtmmp))
				f0 = nm.mcut(i=xxtmmp,f="0:num1,1:num2",nfni=True).mjoin(k="num1",K="num",m=xxmaprev,f="node:%s"%(self.ef1))
				f1 = f0.mjoin(k="num2",K="num",m=xxmaprev,f="node:%s"%(self.ef2)).mcut(f=para).mfsort(f=para).msortf(f=para,o="%s/pair_%s.csv"%(self.outDir,iter))
				f1.run()
				
			# 終了判定
			if(iter>=self.iterMax):
				break

			if iter!=0 and self.same(xxtra,xxprev):
				break

			#MCMD::msgLog("polishing iteration ##{iter} (tra size=#{File.size(xxtra)}")
			os.system("cp %s %s"%(xxtra,xxprev))
			#puts "sspc #{measure} -l #{minSupp} #{xxtra} #{th} #{xxpair}"
			#system "#{CMD_sspc} #{measure} -l #{minSupp} #{xxtra} #{th} #{xxpair}"
			ntsspc.sspc_run(type=self.measure,l=str(self.minSupp),i=xxtra,th=str(self.th),o=xxpair)
			#system "#{CMD_grhfil} ue0 #{xxpair} #{xxtra}"
			ntgrhfil.grhfil_run(type="ue0",i=xxpair,o=xxtra)


			iter+=1


		# 上記iterationで収束したマイクロクラスタグラフを元の節点文字列に直して出力する
		#MCMD::msgLog("converting the numbered nodes into original name ...")
		os.system("tr ' ' ',' < %s > %s"%(xxpair,xxtmmp))

		f0 = nm.mcut(i=xxtmmp,f="0:num1,1:num2",nfni=True)
		f1 = f0.mjoin(k="num1",K="num",m=xxmaprev,f="node:%s"%(self.ef1)).mjoin(k="num2",K="num",m=xxmaprev,f="node:%s"%(self.ef2))
		f2 = f1.mcut(f=para).mfsort(f=para).msortf(f=para,o=self.eo)
		f2.run()

		if(self.no):
			if(self.nf):
				nm.mcut(f="node:%s"%(self.nf),i=xxmap,o=self.no).run()
			else:
				nm.mcut(f="node",i=xxmap,o=self.no).run()


		procTime = datetime.now()-t
		# ログファイル出力
		if(self.logFile):
			kv=[["key","val"]]
			kv.extend(args.getKeyValue())
			kv.append(["iter",str(iter)])
			kv.append(["item",str(procTime)])
			for i in range(len(nSizes)):
				kv.append(["nSize"+str(i),str(nSizes[i])]);
				kv.append(["eSize"+str(i),str(eSizes[i])]);
				kv.append(["dens"+str(i) ,str(denses[i])]);

			nm.writecsv(i=kv,o=self.logFile).run()

	
# 終了メッセージ
		#CMD::endLog(args.cmdline)
		#print(MCMD::endLog(args.cmdline)
	 
if __name__ == '__main__':
	import sys
	args=nu.Margs(sys.argv,"ni=,nf=,ei=,ef=,-indirect,eo=,no=,th=,sim=,sup=,iter=,log=,O=")
	mpolishing(args).run()
	#"ei=,ef=,th="



