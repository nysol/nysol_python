#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import nysol.mcmd as nm
import nysol.util as nu

from nysol.take import extcore as extTake

class mtra2gc(object):

	helpMSG="""
# ver="1.0" # 初期リリース 2016/11/20
# ver="1.1" # resemblanceをjaccardに変更 2016/12/13
# ver="1.2" # sim=の値を変更 2016/12/13
----------------------------
#{$cmd} version #{$version}
----------------------------
概要) トランザクションデータからアイテム類似グラフを構築する。
内容) 2アイテムの共起情報によって類似度を定義し、ある閾値より高い類似度を持つアイテム間に枝を張る。
      mtra2g.rbで可能なclassやtaxonomyの指定は出来ないが、より高速に動作する。
      また類似度の定義にconfidenceを指定可能。
書式) #{$cmd} i= tid= item= [class=] [no=] eo= [s=|S=] [sim=] [th=] [-node_support] [-num] [log=] [T=] [--help] 

  ファイル名指定
  i=     : トランザクションデータファイル【必須】
  tid=   : トランザクションID項目名【必須】
  item=  : アイテム項目名【必須】
  no=    : 出力ファイル(節点)
  eo=    : 出力ファイル(辺:節点ペア)
  log=   : パラメータの設定値をkey-value形式のCSVで保存するファイル名

  【枝を張る条件1】
  s=     : 最小支持度(全トランザクション数に対する割合による指定): 0以上1以下の実数
  S=     : 最小支持度(トランザクション数による指定): 1以上の整数
           S=,s=両方共省略時はs=0.01をデフォルトとする

  【枝を張る条件2:省略可】
  sim=   : アイテムa,bに枝を張る条件として用いる類似度を指定する。
           省略した場合は、最小支持度の条件でのみ枝を張ることになる。
           指定できる類似度は以下の3つのいずれか一つ。
           省略した場合はs=もしくはS=の条件のみで実行される。
             J (jaccard)              : |A ∩ B|/|A ∪ B|
             P (normalized PMI)       : log(|A ∩ B|*|T| / (|A|*|B|)) / log(|A ∩ B|/|T|)
                                        liftを-1〜+1に基準化したもの。
                                        -1:a(b)出現時b(a)出現なし、0:a,b独立、+1:a(b)出現時必ずb(a)出現
             C (Confidence(A=>B))     : |A ∩ B|/|B|
                      A,B: アイテムa(b)を含むトランザクション集合
                      T: 全トランザクション集合
  th=    : sim=で指定された類似度について、ここで指定された値以上のアイテム間に枝を張る。

  【節点条件】
  -node_support : 節点にもS=の条件を適用する。指定しなければ全てのitemを節点として出力する。

  その他
  -num : アイテム項目が正の整数値である場合に指定可能で、処理が高速化される。
  T= : ワークディレクトリ(default:/tmp)
  --help : ヘルプの表示

入力ファイル形式)
トランザクションIDとアイテムの２項目によるトランザクションデータ。
class=を指定する場合は、さらにクラス項目が必要となる。
使用例を参照のこと。

出力形式)
a) 節点ファイル(no=)
例:
  node%0,support,frequency,total
  a,0.6,3,5
  b,0.8,4,5
  c,0.2,1,5
  d,0.8,4,5
  e,0.4,2,5
  f,0.8,4,5
項目の説明:
  node:アイテム
  support:frequency/total
  frequency:アイテムの出現頻度
  total:全トランザクション数

b) 枝ファイル(eo=)
例:
  node1%0,node2%1,frequency,frequency1,frequency2,total,support,confidence,lift,jaccard,PMI
  a,b,3,3,4,5,0.6,1,1.25,0.75,0.4368292054
  a,c,1,1,3,5,0.2,1,1.666666667,0.3333333333,0.3173938055
項目の説明:
  node1,node2:アイテム
  support:frequency/total
  frequency:2つのアイテム(node1,node2)の共起頻度
  frequency1:node1の出現頻度
  frequency2:node2の出現頻度
  total:全トランザクション数
  confidence: frequency/frequency1
  lift: (total*frequency)/(frequency1*frequency2)
  jaccard,PMI:上述の「枝を張る条件2」を参照

基本的な使用例)
$ cat tra1.csv 
id,item
1,a
1,b
1,c
1,f
2,d
2,e
2,f
3,a
3,b
3,d
3,f
4,b
4,d
4,f
5,a
5,b
5,d
5,e
$ #{$cmd} i=tra.csv tid=id item=item S=1 sim=C th=0.7 no=node.csv eo=edge.csv
$ cat node.csv 
node%0,support,frequency,total
a,0.6,3,5
b,0.8,4,5
c,0.2,1,5
d,0.8,4,5
e,0.4,2,5
f,0.8,4,5
$ cat edge.csv
node1%0,node2%1,frequency,frequency1,frequency2,total,support,confidence,lift,jaccard,PMI
a,b,3,3,4,5,0.6,1,1.25,0.75,0.4368292054
b,a,3,4,3,5,0.6,0.75,1.25,0.75,0.4368292054
b,d,3,4,4,5,0.6,0.75,0.9375,0.6,-0.1263415893
b,f,3,4,4,5,0.6,0.75,0.9375,0.6,-0.1263415893
c,a,1,1,3,5,0.2,1,1.666666667,0.3333333333,0.3173938055
c,b,1,1,4,5,0.2,1,1.25,0.25,0.1386468839
c,f,1,1,4,5,0.2,1,1.25,0.25,0.1386468839
d,b,3,4,4,5,0.6,0.75,0.9375,0.6,-0.1263415893
d,f,3,4,4,5,0.6,0.75,0.9375,0.6,-0.1263415893
e,d,2,2,4,5,0.4,1,1.25,0.5,0.2435292026
f,b,3,4,4,5,0.6,0.75,0.9375,0.6,-0.1263415893
f,d,3,4,4,5,0.6,0.75,0.9375,0.6,-0.1263415893

# Copyright(c) NYSOL 2012- All Rights Reserved.
		"""

	verInfo="version=1.2"


	paramter = {	
		"i":"filename",
		"no":"filename",
		"eo":"filename",
		"log":"filename",
		"tid":"fld",
		"item":"fld",
		"s":"int",
		"S":"int",
		"sim":"str",
		"th":"float",
		"node_support":"bool",
		"T=":"str",
		"num":"bool"
	}

	paramcond = {	
		"hissu": ["i","tid","item","eo"]
	}

	def help():
		print(mtra2gc.helpMSG) 

	def ver():
		print(mtra2gc.versionInfo)

	def __param_check_set(self , kwd):

		for k,v in kwd.items():
			if not k in mtra2gc.paramter	:
				raise( Exception("KeyError: {} in {} ".format(k,self.__class__.__name__) ) )
			#型チェック入れる

		self.msgoff = True

		self.iFile   = kwd["i"]
		self.oeFile  = kwd["eo"]
		self.idFN    = kwd["tid"]  
		self.itemFN  = kwd["item"] 

		self.onFile  = kwd["no"]        if "no"  in kwd else None
		self.logFile = kwd["log"]       if "log" in kwd else None
		self.sim     = kwd["sim"]       if "sim" in kwd else None
		self.th      = float(kwd["th"]) if "th"  in kwd else None # 類似度measure


		self.node_support = kwd["node_support"] if "node_support" in kwd else False
		self.num = kwd["num"] if "num" in kwd else False

		# 最小サポート件数
		self.minSupPrb = float(kwd["s"])  if "s" in kwd else None
		self.minSupCnt = float(kwd["S"])  if "S" in kwd else None

		if self.minSupPrb==None and self.minSupCnt==None :
			self.minSupPrb=0.01 

		if self.sim and "JPC".find(self.sim)==-1:
			raise Exception("sim= takes one of 'J','P','C'")




	def __init__(self,**kwd):

		#パラメータチェック
		self.args = kwd
		self.__param_check_set(kwd)


	def convN(self,iFile,idFN,itemFN,oFile,mapFile):


		f0 = nm.mcut(f=itemFN+":##item",i=iFile).mcount(k="##item",a="##freq",o=mapFile)
		f0.run()

		extTake.mmaketra(i=iFile,o=oFile,f=itemFN,k=idFN)
		#f1 = nm.mtra(k=idFN,f=itemFN+":##num",i=iFile).mcut(f="##num",nfno=True,o=oFile)


		#nm.runs([f0,f1])
		# #{mapFile}"
		# ##item,##freq%0nr,##num
		# b,4,0
		# d,4,1
		return nu.mrecount(i=oFile,nfn=True)

	def conv(self,iFile,idFN,itemFN,oFile,mapFile):
		temp=nu.Mtemp()
		xxtra=temp.file()

		# 入力ファイルのidがnilの場合は連番を生成して新たなid項目を作成する。
		f0   = nm.mcut(f=itemFN+":##item",i=iFile)
		f0 <<= nm.mcount(k="##item",a="##freq")
		f0 <<= nm.mnumber(s="##freq%nr",a="##num",o=mapFile)
		f0.run()

		f1   = nm.mjoin(k=itemFN,K="##item",f="##num",m=mapFile,i=iFile,o=xxtra)
		f1.run()
		
		#f1 <<= nm.mtra(k=idFN,f="##num")
		#f1 <<= nm.mnumber(q=True,a="##traID")
		#f1 <<= nm.mcut(f="##num",nfno=True,o=oFile)

		extTake.mmaketra(i=xxtra,o=oFile,f="##num",k=idFN)

		#f1.run()

		# ##item,##freq%0nr,##num
		# b,4,0
		# d,4,1
		size=nu.mrecount(i=oFile,nfn=True)

		return size


	def run(self):

		from datetime import datetime	
		t = datetime.now()

		temp=nu.Mtemp()
		xxsspcin=temp.file()
		xxmap=temp.file()

		# traファイルの変換とマップファイルの作成
		if self.num :
			total = self.convN(self.iFile,self.idFN,self.itemFN,xxsspcin,xxmap)
		else:
			total = self.conv(self.iFile,self.idFN,self.itemFN,xxsspcin,xxmap)

		# system "head xxsspcin"
		# 3 5 0 2
		# 4 1 2
		# 0 2 3 1
		# 1 0 2
		# 3 4 0 1
		# system "head xxmap"
		# ##item,##freq%0nr,##num
		# b,4,0
		# d,4,1
		# f,4,2
		minSupp = int(total*self.minSupPrb)	if self.minSupPrb else self.minSupCnt
			

		# sspc用simの文字列
		if self.sim :
			if self.sim=="J":
				sspcSim="R"
			elif self.sim=="P":
				sspcSim="P"
			elif self.sim=="C":
				sspcSim="i"
		# sim=省略時はRでth=0とする(sim制約なし)
		else:
			sspcSim="R"
			self.th=0

		############ 列挙本体 ############
		xxsspcout=temp.file()
		tpstr =  sspcSim+"ft_" if self.msgoff else sspcSim+"ft"
		extTake.sspc(type=tpstr,TT=minSupp,i=xxsspcin,th=self.th,o=xxsspcout)

		##################################

		xxtmmp=temp.file()
		
		f =   nm.mread(i=xxsspcout) 
		f <<= nm.cmd("tr ' ()' ','") 
		f <<= nm.mcut(f="1:i1,2:i2,0:frequency,4:sim",nfni=True)

		if self.num :

			f <<= nm.mfldname(f="i1:node1,i2:node2")
			if self.sim!="C":
				f <<= nm.mfsort(f="node1,node2")
			
			f <<= nm.mjoin(k="node1",K="##item",m=xxmap,f="##freq:frequency1")
			f <<= nm.mjoin(k="node2",K="##item",m=xxmap,f="##freq:frequency2") 
			
		else:

			f <<= nm.mjoin(k="i1",K="##num",m=xxmap,f="##item:node1,##freq:frequency1")
			f <<= nm.mjoin(k="i2",K="##num",m=xxmap,f="##item:node2,##freq:frequency2") 

			if self.sim!="C":

				f <<= nm.mcut(f="i1,i2,frequency,sim,node1,node2,frequency1,frequency2,node1:node1x,node2:node2x")
				f <<= nm.mfsort(f="node1x,node2x")
				f <<= nm.mcal(c='if($s{node1}==$s{node1x},$s{frequency1},$s{frequency2})',a="freq1")
				f <<= nm.mcal(c='if($s{node2}==$s{node2x},$s{frequency2},$s{frequency1})',a="freq2")
				f <<= nm.mcut(f="i1,i2,frequency,sim,node1x:node1,node2x:node2,freq1:frequency1,freq2:frequency2")

		f <<= nm.msetstr(v=total,a="total")
		f <<= nm.mcal(c='${frequency}/${frequency1}',a="confidence")
		f <<= nm.mcal(c='${frequency}/${total}',a="support")
		f <<= nm.mcal(c='${frequency}/(${frequency1}+${frequency2}-${frequency})',a="jaccard")
		f <<= nm.mcal(c='(${frequency}*${total})/((${frequency1}*${frequency2}))',a="lift")
		f <<= nm.mcal(c='(ln(${frequency})+ln(${total})-ln(${frequency1})-ln(${frequency2}))/(ln(${total})-ln(${frequency}))',a="PMI")
		f <<= nm.mcut(f="node1,node2,frequency,frequency1,frequency2,total,support,confidence,lift,jaccard,PMI")
		f <<= nm.msortf(f="node1,node2",o=self.oeFile)
		f.run()

		if self.onFile:
			f4 =   nm.mcut(f=self.itemFN+":node",i=self.iFile)
			f4 <<= nm.mcount(k="node",a="frequency")
			if self.node_support :
				minstr = "[%s,]"%(minSupp)
				f4 <<= nm.mselnum(f="frequency",c=minstr)

			f4 <<= nm.msetstr(v=total,a="total")
			f4 <<= nm.mcal(c='${frequency}/${total}',a="support")
			f4 <<= nm.mcut(f="node,support,frequency,total",o=self.onFile)
			f4.run()

		procTime=datetime.now()-t

		# ログファイル出力
		if self.logFile :
			kv=[["key","value"]]
			for k,v in self.args.items():
				kv.append([k,str(v)])
			kv.append(["time",str(procTime)])
			nm.writecsv(i=kv,o=self.logFile).run()



