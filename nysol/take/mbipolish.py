#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import os.path
import shutil

import nysol.mod as nm
import nysol.util.margs as margs
import nysol.util.mtemp as mtemp
import nysol.util.mmkdir as mkDir
import nysol.util.mrecount as mrecount

from nysol.take import extcore as extTake


class mbipolish(object):

	helpMSG="""
----------------------------
#{$cmd} version #{$version}
----------------------------
概要) 2部グラフの研磨をおこなう。
内容) 2部グラフを入力として、密度の高い2部部分グラフにあって枝が張られていないノードペアに枝を張る。
      逆に、密度の低い2部部分グラフにあって枝が張られているノードペアの枝を刈る。
      新たに張られる枝や刈られる枝の程度は、sim=,th=とsim2,th2で与えた値によって変わる。
書式) #{$cmd} ei= [ef=] [nf=] eo= [sim=i|I|s|S|T|R|P|C] th= [th2=] [sim2=i|I|s|S|T|R|P|C] [sup=] [iter=] [log=] [T=] [--help]

  ファイル名指定
  ei=    : 枝データファイル
  ef=    : 枝データ上の2つの節点項目名(省略時は"node1,node2")
  eo=    : データ研磨後の枝データファイル
  sim|2= : 節点a,bと接続された枝集合を、それぞれA,Bとすると、節点a,bに枝を張るために用いる類似度。
           省略時はRが設定される。(sim2のデフォルト:sim=)
             i: inclusion
             I: both-inclusion
             S: |A∩B|/max(|A|,|B|)
             s: |A∩B|/min(|A|,|B|)
             T (intersection): find pairs having common [threshld] items
             R (resemblance): find pairs s.t. |A\capB|/|A\cupB| >= [threshld]
             P (PMI): find pairs s.t. log (|A\capB|*|all| / (|A|*|B|)) >= [threshld]
             C (cosine distance): find pairs s.t. inner product of their normalized vectors >= [threshld]
  th|2=  : sim|2=で指定された類似度について、ここで指定された値以上の節点間に枝を張る。(th2のデフォルト:th=)
  sup=   : 左の部の次数がsup以上のノードを対象とする。省略すればsup=0。
  kn|2 = : kn=で指定された値以上の共起頻度を対象とする。kn2=で指定された値以上の次数を持つ右部を対象とする。
           省略すればkn=1,kn2=1 [1以上の整数]
  iter=  : データ研磨の最大繰り返し数(デフォルト=30)
  log=   : ディレクトリ内にパラメータの設定値や収束回数等をkey-value形式のCSVで出力.繰り返し毎に生成される類似グループ出力


  その他
  T= : ワークディレクトリ(default:/tmp)
  --help : ヘルプの表示

備考)
内部で起動しているコマンドsspcは0から始まる整数で指定された節点名を前提として処理する。
一方で本コマンドは、任意の文字列で節点名を表したデータを処理できる。
それは、sspcを実行する前に、それら文字列と整数との対応表を前処理で作成しているからである。

例) 2部グラフデータからのみグラフ研磨を実行する例
$ cat edge.csv
node1,node2
A,a
A,b
B,a
B,b
C,c
C,d
D,b
D,e

$ #{$cmd} ei=edge.csv ef=n1,n2 th=0.2 eo=output.csv
#MSG# converting the numbered nodes into original name ...; 2015/09/27 21:59:08
#END# #{$cmd} ei=edge.csv ef=n1,n2 th=0.2 eo=output.csv; 2015/09/27 21:59:08
$ cat output.csv
n1,n2
A,a
A,b
A,e
B,a
B,b
B,e
C,c
C,d
D,a
D,b
D,e

# Copyright(c) NYSOL 2012- All Rights Reserved.
		"""

	verInfo="version=1.2"
	
	def help():
		print(mbipolish.helpMSG) 

	def ver():
		print(mbipolish.verInfo)




	def __init__(self,args):

		self.oFile        = args.file("o=", "w")
		self.ei           = args.file("ei=","r") # edgeファイル名
		self.ef1,self.ef2 = args.field("ef=", self.ei, "node1,node2",2,2)["names"]
		self.eo      = args.file("eo=", "w")
		self.logDir  = args.file("log=", "w")
		self.outDir  = args.str("O=")	# 過程出力

		self.measure  = args.str("sim=","R")        # similarity measure
		self.measure2 = args.str("sim2=",self.measure)   # similarity measure
		self.minSupp  = args.int("sup=",0)          # minimam support
		self.iterMax  = args.int("iter=",30,1)      # upper bound of iterations
		self.th       = args.float("th=")           # threashold for similarity measure
		self.th2      = args.float("th2=",self.th)       # threashold for similarity measure
		self.kn       = args.float("kn=",1)         # no. of interaction size more than threshold 
		self.kn2      = args.float("kn2=",1)         # no. of right node size more than threshold 

		if self.outDir:
			mkDir(outDir,rm=True) 

		if self.logDir:
			mkDir(logDir,rm=True) 

		self.__tempW	= mtemp.Mtemp()
		self.part1 = self.__tempW.file()
		self.part2 = self.__tempW.file()



	def calGsize(self,file):
		edgesize= mrecount.mrecount(i=file,nfni=True)

		f=None
		f <<= nm.mcut(i=file,f="0:tra",nfni=True)
		f <<= nm.mtra(f="tra")
		f <<= nm.muniq(k="tra")
		f <<= nm.mcount(a="cnt")
		f <<= nm.mcut(f="cnt")
		f <<= nm.writelist(dtype="cnt:int")
		nodesize = f.run()[0][0]
		print(nodesize)
		return nodesize,edgesize
	
	def same(self,file1,file2):

		xx = self.__tempW.file()

		if os.path.getsize(file1)!=os.path.getsize(file2):
			return False

		os.system("diff -q %s %s > %s"%(file1,file2,xx))

		if os.path.getsize(xx)!=0:
			return False 
	
		return True




	def edge2mtx(self,ei,itra):
		#MCMD::msgLog("converting graph files into a pair of numbered nodes ...")
		wff1=self.__tempW.file()
		
		wf1 = nm.mcut(f="{}:node".format(self.ef1),i=ei) 
		wf2 = nm.mcut(f="{}:node".format(self.ef2),i=ei)
		
		p1   = nm.mcut(f=self.ef1,i=ei) 
		p1 <<= nm.muniq(k=self.ef1)
		p1 <<= nm.mdelnull(f=self.ef1)
		p1 <<= nm.mnumber(q=True,a="num1",S=1,o=self.part1)

		p2   = nm.mcut(f=self.ef2,i=ei) 
		p2 <<= nm.muniq(k=self.ef2)
		p2 <<= nm.mdelnull(f=self.ef2)
		p2 <<= nm.mnumber(q=True,a="num2",S=1,o=self.part2)
		
		runp = None
		runp <<= nm.mcut(f=[self.ef1,self.ef2],i=ei)
		runp <<= nm.mjoin(k=self.ef1,m=p1,f="num1")
		runp <<= nm.mjoin(k=self.ef2,m=p2,f="num2")
		runp <<= nm.mcut(f="num1,num2")
		runp <<= nm.mtra(k="num1",f="num2")
		runp <<= nm.msortf(f="num1%n")
		
		#これだとおかしい
		#runp <<= nm.mcut(f="num2",nfno=True)
		#runp <<= nm.cmd("tr ',' ' ' > %s"%(itra))

		runp <<= nm.mcut(f="num2",nfno=True,o=wff1)
		runp.run()
		os.system("tr ',' ' ' < {} > {}".format(wff1,itra))

		
	def noPat(self):
		##MCMD::msgLog("There is no frequent item. The value is too large")
		exit()
	
	def convRsl(self,ifile,ofile,logDir=None):
		# 上記iterationで収束したマイクロクラスタグラフを元の節点文字列に直して出力する
		#MCMD::msgLog("converting the numbered nodes into original name ...")
		f = None
		f <<= nm.mcut(nfni=True,f="0:tra",i=ifile)
		f <<= nm.msed(f="tra", c=' $',v="")
		f <<= nm.mnumber(q=True,S=1,a="num1")
		f <<= nm.mtra(r=True,f="tra:num2")
		f <<= nm.mjoin(k="num2",m=self.part2,f=self.ef2)
		f <<= nm.mjoin(k="num1",m=self.part1,f=self.ef1)
		f <<= nm.msortf(f="num1%n,num2%n")
		f <<= nm.mcut(f=[self.ef1,self.ef2])

		if logDir:
			f <<= nm.mfldname(q=True , o="{}/#{ofile}".format(logDir,ofile))
		else:
			f <<= nm.mfldname(q=True,o=ofile)

		f.run()

	def convSim(self,ifile,ofile,logDir):
		f = None
		f <<= nm.mcut(nfni=True,f="0:tra",i=ifile)
		f <<= nm.msed(f="tra", c=' $',v="")
		f <<= nm.mnumber(q=True,S=1,a="num1")
		f <<= nm.mtra(r=True,f="tra:num11")
		f <<= nm.mnumber(q=True,S=1,a="order")
		f <<= nm.mcal(c='${num11}+1',a="num1")
		f <<= nm.mjoin(k="num1",m=self.PART1,f=self.ef1)
		f <<= nm.mtra(k="num0",s="order%n,num1%n",f=self.ef1)
		f <<= nm.mcut(f=self.ef1,o="{}/{}".format(logDir,ofile))
		f.run()

	# execute
	def run(self):
		from datetime import datetime	
		t = datetime.now()

		xxinp  = self.__tempW.file()
		xxitra = self.__tempW.file()
		xxprev = self.__tempW.file()
		xxpair = self.__tempW.file()
		xxtra  = self.__tempW.file()
		xxdiff = self.__tempW.file()

		self.edge2mtx(self.ei,xxinp)
		extTake.grhfil(type='D""',i=xxinp,o=xxitra)
		
		iter=0
		while True :
			# 終了判定
			if iter>=self.iterMax:
				break
			if iter!=0 and self.same(xxitra,xxprev):
				break

			#MCMD::msgLog("polishing iteration ##{iter} (tra size=#{File.size(xxitra)}")
			shutil.copyfile(xxitra,xxprev)
		
			nodeSize,edgeSize=self.calGsize(xxitra)
			edgeSize1 = edgeSize+1

			#print "sspc #{measure} -l #{minSupp} #{xxtra} #{th} #{xxpair}"
			runpara={}
			runpara["type"] = "t"+self.measure 
			runpara["T"] = self.kn
			runpara["l"] = self.minSupp
			runpara["U"] = 100000
			runpara["L"] = 1
			runpara["i"] = xxitra
			runpara["th"] = self.th
			runpara["o"] = xxpair

			extTake.sspc(runpara)
			#	puts   "sspc t#{measure} -T #{kn} -l #{minSupp} -U 100000 -L 1 #{xxitra} #{th} #{xxpair}"

			if not os.path.exists(xxpair):
				self.noPat()
			if os.path.getsize(xxpair) == 0:
				self.noPat()
			
			extTake.grhfil(type='eu0',i=xxpair,o=xxtra)
		
			if self.logDir :
				self.convSim(xxtra,"simGp{}.csv".format(iter),self.logDir)
			
			os.system("cat {} {} > {}".format(xxitra,xxtra,xxpair))
		
			runpara={}
			runpara["type"] = self.measure2
			runpara["T"] = self.kn2
			runpara["c"] = edgeSize
			runpara["i"] = xxpair
			runpara["th"] = self.th2
			runpara["o"] = xxtra

			extTake.sspc(runpara)

			if not os.path.exists(xxtra):
				self.noPat()
			if os.path.getsize(xxtra) == 0:
				self.noPat()

			extTake.grhfil(type='ed',i=xxtra,o=xxpair)
			os.system("tail -n +{} {} > {}".format(edgeSize1,xxpair,xxtra))

			extTake.grhfil(type='D',i=xxtra,o=xxpair)

			extTake.grhfil(type='DE',d=xxitra,i=xxpair,o=xxdiff)

			shutil.copyfile(xxpair,xxitra)

			if self.logDir :
				extTake.grhfil(type='D',i=xxtra,o=xxpair)
				self.convRsl(xxpair,"iter{}.csv".format(iter),logDir)
		
			iter+=1


		extTake.grhfil(type='D',i=xxitra,o=xxpair)
		self.convRsl(xxpair,self.eo)

		procTime = datetime.now()-t

		if self.logDir :

			kv=[["key","value"]]
			kv.extend(self.args.getKeyValue())
			kv.append(["iter",str(iter)])
			kv.append(["time",str(procTime)])
			for i in range(len(nSizes)):
				kv.append(["nSize"+str(i),str(nSizes[i])]);
				kv.append(["eSize"+str(i),str(eSizes[i])]);
				kv.append(["dens"+str(i) ,str(denses[i])]);

			nm.writecsv(i=kv,o="{}/keyVal.csv".format(self.logDir)).run()



if __name__ == '__main__':
	import sys
	args=margs.Margs(sys.argv,"ei=,ef=,eo=,th=,sim=,th2=,sim2=,kn=,kn2=,sup=,iter=,log=","ei=,ef=,th=")
	mbipolish(args).run()
