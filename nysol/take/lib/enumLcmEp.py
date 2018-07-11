#!/usr/bin/env python
# -*- coding: utf-8 -*-
#require "rubygems"
#require "nysol/mcmd"
#require "nysol/take"

#require "nysol/traDB.rb"

#module TAKE
import os
import shutil
import nysol.mcmd as nm
import nysol.util.mtemp as mtemp
from nysol.take import extcore as extTake
import nysol.vsop._vsoplib as VSOP

#========================================================================
# 列挙関数:lcm 利用DB:TraDB
#========================================================================
class LcmEp(object):
	#未実装zddつくってから
	def reduceTaxo(self,pat,items):
		#tf=mtemp.Mtemp()

		if items.taxonomy==None:
			return pat

		#xxrt = tf.file
		taxo=items.taxonomy
		xxrt = nm.mtrafld(f=taxo.itemFN+","+taxo.taxoFN ,valOnly=True,a="__fld",i=taxo.file).mcut(f="__fld")

		# xxrtの内容：oyakoに親子関係にあるアイテム集合のリストが格納される
		# __fld
		# A X
		# B X
		# C Y
		# D Z
		# E Z
		# F Z
		oyako=VSOP.constant(0)

		for fldVal in xxrt:
			oyako=oyako+VSOP.itemset(fldVal[0])
		# 親子リストにあるアイテム集合を含むパターンを削除する
		pat=pat.restrict(oyako).iif(0,pat)

		return pat



	intMax=2147483646

	# posトランザクションの重み計算
	# マニュアルの式(10)
	def calOmega(self,posCnt):
		return LcmEp.intMax/posCnt

	# LCM最小サポートの計算
	# マニュアルの式(9)
	def calSigma(self,minPos,minGR,posCnt,negCnt):
		omegaF=float(LcmEp.intMax)/float(posCnt)
		beta=minPos
		w=float(posCnt)/float(negCnt)

		#print("omegaF="+str(omegaF))
		#print("minPos="+str(minPos))
		#print("beta="+str(beta))
		#print("posCnt="+str(posCnt))
		#print("negCnt="+str(negCnt))
		#print("minGR="+str(minGR))
		#print("w="+str(w))

		sigma=int(beta*(omegaF-w/minGR))  # 切り捨て
		if sigma<=0:
			sigma=1 
		return sigma

	def __init__(self,db,outtf=True):
		self.size  = None
		self.pFile = None
		self.tFile = None
		self.temp  = mtemp.Mtemp()
		self.db    = db # 入力データベース
		self.file  = self.temp.file()
		self.outtf = outtf
		self.weightFile = {}
		self.posWeight  = {}
		self.sigma      = {}
		self.msgoff = True
		
		items      = self.db.items
		for cName,posSize in db.clsNameRecSize.items(): 
			self.weightFile[cName] = self.temp.file()
			self.posWeight[cName]  = self.calOmega(posSize)
			f =   nm.mcut(nfno=True,f=self.db.clsFN,i=self.db.cFile)
			f <<= nm.mchgstr(nfn=True,f=0,O=-1,o=self.weightFile[cName],c="%s:%s"%(cName,self.posWeight[cName]))
			f.run()

		# アイテムをシンボルから番号に変換する。
		f = nm.mjoin(k=self.db.itemFN,K=items.itemFN,i=self.db.file,m=items.file,f=items.idFN)
		f <<= nm.mcut(f=self.db.idFN+","+items.idFN)
		f <<= nm.mtra(k=self.db.idFN,f=items.idFN)
		f <<= nm.mcut(f=items.idFN,nfno=True,o=self.file)
		f.run()


	# 各種パラメータを与えて列挙を実行
	def enumerate(self,eArgs):

		pFiles=[]
		tFiles=[]
		tf=mtemp.Mtemp()
		for cName,posSize in self.db.clsNameRecSize.items(): 
			negSize=self.db.traSize-posSize
			if "minGR" in eArgs :
				self.minGR = eArgs["minGR"]
			else:
				minProb = eArgs["minProb"] if ( "minProb" in eArgs ) else 0.5
				if "uniform" in eArgs and eArgs["uniform"] == True:
					self.minGR = (minProb/(1-minProb)) * (self.db.clsSize-1) # マニュアルの式(4)
				else:
					self.minGR = (minProb/(1-minProb)) * (float(negSize)/float(posSize)) # マニュアルの式(4)


			# 最小サポートと最小サポート件数
			# s=0.05
			# s=c1:0.05,c2:0.06
			# S=10
			# S=c1:10,c2:15
			if "minCnt" in eArgs :
				if isinstance(eArgs["minCnt"],dict):
					self.minPos = eArgs["minCnt"][cName]
				else:
					self.minPos = eArgs["minCnt"]
			else:
				if isinstance(eArgs["minSup"],dict):
					self.minPos = int(eArgs["minSup"][cName] * float(posSize) + 0.99)
				else:
					self.minPos = int(eArgs["minSup"] * flost(posSize) + 0.99)

			# 最大サポートと最大サポート件数
			if "maxCnt" in eArgs:
				if isinstance(eArgs["maxCnt"],dict):
					self.maxPos = eArgs["maxCnt"][cName]
				else:
					self.maxPos = eArgs["maxCnt"]

			elif "maxSup" in eArgs:
				if isinstance(eArgs["maxSup"],dict):
					self.maxPos = int(eArgs["maxSup"][cName] * float(posSize) + 0.99)
				else:
					self.maxPos = int(eArgs["maxSup"] * float(posSize) + 0.99)
			else:
				self.maxPos = None


			self.sigma[cName] = self.calSigma(self.minPos,self.minGR,posSize,negSize)

			# lcmのパラメータ設定と実行
			# 頻出パターンがなかった場合、lcm出力ファイルが生成されないので
			# そのときのために空ファイルを生成しておいく。
			lcmout = tf.file() # lcm出力ファイル
			with open(lcmout, "w") as efile:
				pass


			runPara={}			

			if self.msgoff:
				runPara["type"] = eArgs["type"]+"IA_"
			else:
				runPara["type"] = eArgs["type"]+"IA"

			#if self.maxPos: #rubyだとif @maxCntなってる（どこにも設定されてないので）動いてないはず
			if self.maxPos:
				runPara["U"] = self.maxPos

			if "minLen" in eArgs:
				runPara["l"] = str(eArgs["minLen"])

			if "maxLen" in eArgs:
				runPara["u"] = str(eArgs["maxLen"])

			runPara["w"] = self.weightFile[cName]

			runPara["i"] = self.file

			runPara["sup"] = str(self.sigma[cName])

			runPara["o"] = lcmout

			# lcm実行
			#MCMD::msgLog("#{run}")
			#TAKE::run_lcm(run)
			#print(self.sigma)
			#print(runPara)
			#MCMD::msgLog("output patterns to CSV file ...")

			extTake.lcm(runPara)

			pFiles.append(self.temp.file())

			transle = tf.file()
			extTake.lcmtrans(lcmout,"e",transle)

			f =   nm.mdelnull(f="pattern",i=transle)
			f <<= nm.mcal(c='round(${countN},1)',a="neg")
			f <<= nm.mcal(c='round(${countP}/%s,1)'%(self.posWeight[cName]),a="pos")
			f <<= nm.mdelnull(f="pattern") #いる？
			f <<= nm.msetstr(v=cName,a="class")
			f <<= nm.msetstr(v=posSize,a="posTotal")
			f <<= nm.msetstr(v=self.minGR,a="minGR")
			f <<= nm.mcut(f="class,pid,pattern,size,pos,neg,posTotal,minGR",o=pFiles[-1])
			f.run()

			#s = nutil.mrecount(i=self.file)
			#MCMD::msgLog("the number of contrast patterns on class `#{cName}' enumerated is #{s}")

			if self.outtf :
				# トランザクション毎に出現するパターンを書き出す
				#MCMD::msgLog("output tid-patterns ...")
				tFiles.append(self.temp.file())
				xxw= tf.file()
				
				xxw =   nm.mcut(f=self.db.idFN,i=self.db.file)
				xxw <<= nm.muniq(k=self.db.idFN)
				xxw <<= nm.mnumber(S=0,a="__tid",q=True)

				translt = self.temp.file()
				extTake.lcmtrans(lcmout,"t",translt)

				f =   nm.mjoin(k="__tid",m=xxw,f=self.db.idFN,i=translt)
				f <<= nm.msetstr(v=cName,a="class")
				f <<= nm.mcut(f=self.db.idFN+",class,pid",o=tFiles[-1])
				f.run()


		# クラス別のパターンとtid-pidファイルを統合して最終出力
		self.pFile = self.temp.file()
		self.tFile = self.temp.file()

		# パターンファイル併合
		xxpCat = tf.file()
		f =   nm.mcat(i=",".join(pFiles))
		f <<= nm.msortf(f="class,pid")
		f <<= nm.mnumber(s="class,pid",S=0,a="ppid",o=xxpCat)
		f.run()

		# パターンファイル計算
		items=self.db.items
		f =   nm.mcut(f="class,ppid:pid,pattern,size,pos,neg,posTotal,minGR",i=xxpCat)
		f <<= nm.msetstr(v=self.db.traSize,a="total")
		f <<= nm.mcal(c='${total}-${posTotal}',a="negTotal") # negのトータル件数
		f <<= nm.mcal(c='${pos}/${posTotal}',a="support") # サポートの計算
		f <<= nm.mcal(c='if(${neg}==0,1.797693135e+308,(${pos}/${posTotal})/(${neg}/${negTotal}))',a="growthRate")

		if "uniform" in eArgs and eArgs["uniform"] == True :
			f <<= nm.mcal(c='(${pos}/${posTotal})/(${pos}/${posTotal}+(%s-1)*${neg}/${negTotal})'%(self.db.clsSize),a="postProb")
		else:
			f <<= nm.mcal(c='${pos}/(${pos}+${neg})',a="postProb")

		f <<= nm.msel(c='${pos}>=%s&&${growthRate}>=${minGR}'%(self.minPos)) # minSupとminGRによる選択
		f <<= nm.mvreplace(vf="pattern",m=items.file,K=items.idFN,f=items.itemFN)
		f <<= nm.mcut(f="class,pid,pattern,size,pos,neg,posTotal,negTotal,total,support,growthRate,postProb")
		f <<= nm.mvsort(vf="pattern")
		f <<= nm.msortf(f="class%nr,postProb%nr,pos%nr",o=self.pFile)
		f.run()

		# アイテムを包含している冗長なタクソノミを削除
		if items.taxonomy:
			#MCMD::msgLog("reducing redundant rules in terms of taxonomy ...")
			##ここは後で
			zdd=VSOP.constant(0)
			dt = nm.mcut(i=self.pFile,f="pattern")

			for fldVal in dt:
				zdd=zdd+VSOP.itemset(fldVal[0])

			zdd=self.reduceTaxo(zdd,self.db.items)

			xxp1=tf.file()
			xxp2=tf.file()
			xxp3=tf.file()
			zdd.csvout(xxp1)
			
			nm.mcut(nfni=True,f="1:pattern",i=xxp1).mvsort(vf="pattern").msortf(f="pattern",o=xxp2).run()
			nm.msortf(f="pattern",i=self.pFile).mcommon(k="pattern",m=xxp2).msortf(f="class%nr,postProb%nr,pos%nr",o=xxp3).run()
			shutil.move(xxp3,self.pFile)

		

		if self.outtf:
			# 列挙されたパターンを含むtraのみ選択するためのマスタ
			xxp4=nm.mcut(f="class,pid",i=self.pFile)
			f =   nm.mcat(i=",".join(tFiles))
			f <<= nm.mjoin(k="class,pid",m=xxpCat,f="ppid") # 全クラス統一pid(ppid)結合
			f <<= nm.mcommon(k="class,ppid",K="class,pid",m=xxp4) # 列挙されたパターンの選択
			f <<= nm.mcut(f=self.db.idFN+",class,ppid:pid")
			f <<= nm.msortf(f=self.db.idFN+",class,pid",o=self.tFile)
			f.run()

		#self.size = nutil.mrecount(i=self.pFile)
		#MCMD::msgLog("the number of emerging patterns enumerated is #{@size}")


	def output(self,outpath):
		shutil.move(self.pFile,outpath+"/patterns.csv")
		if self.outtf:
			shutil.move(self.tFile,outpath+"/tid_pats.csv")
