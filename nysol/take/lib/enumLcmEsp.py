#!/usr/bin/env python
# -*- coding: utf-8 -*-
#/* ////////// LICENSE INFO ////////////////////
#
# * Copyright (C) 2013 by NYSOL CORPORATION
# *
# * Unless you have received this program directly from NYSOL pursuant
# * to the terms of a commercial license agreement with NYSOL, then
# * this program is licensed to you under the terms of the GNU Affero General
# * Public License (AGPL) as published by the Free Software Foundation,
# * either version 3 of the License, or (at your option) any later version.
# * 
# * This program is distributed in the hope that it will be useful, but
# * WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF 
# * NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# *
# * Please refer to the AGPL (http://www.gnu.org/licenses/agpl-3.0.txt)
# * for more details.
#
# ////////// LICENSE INFO ////////////////////*/
import os
import shutil
import nysol.util as nu
import nysol.mcmd as nm
from nysol.take import extcore as extTake


#========================================================================
# 列挙関数:lcm_seq 利用DB:SeqDB
#========================================================================
class LcmEsp(object):

	intMax=2147483646

	# posトランザクションの重み計算
	# マニュアルの式(10)
	def calOmega(self,posCnt):
		return LcmEsp.intMax/posCnt


	# LCM最小サポートの計算
	# マニュアルの式(9)
	def calSigma(self,minPos,minGR,posCnt,negCnt):
		omegaF=float(LcmEsp.intMax)/float(posCnt)
		beta=minPos
		w=float(posCnt)/float(negCnt)
		sigma=int(beta*(omegaF-w/minGR))  # 切り捨て
		if sigma<=0:
			sigma=1 
		return sigma


	# 1:β   最小支持度
	# 2:γ   最小GR
	# 3:Dp  クラス1トランザクション数
	# 4:Dn  クラス2トランザクション数
	# 5:lcmq   LCM列挙数調整パラメータ
  # 1:γ 2:Dp 3:Dn 4:lcmq 
  # 未使用
	#def calOmega(minGR,posCnt,negCnt,lcmq=0.5)
	#	#return (negCnt.to_f*(1-prob))/(posCnt.to_f*prob)/(1-lcmq)
	#	return negCnt.to_f/posCnt.to_f/minGR.to_f/(1-lcmq)
	#end
	#def calAlphaD(minSup,minGR,posCnt,negCnt,lcmq=0.5)
	#	return lcmq*minSup.to_f*negCnt.to_f/minGR.to_f/(1-lcmq)
	#end


	def __init__(self,db,outtf=True):
		self.size  = None
		self.msgoff = True

		self.temp  = nu.Mtemp()
		self.db    = db # 入力データベース
		self.file  = self.temp.file()
		self.outtf = outtf
		items      = self.db.items

		# 重みファイルの作成
		# pos,negのTransactionオブジェクトに対してLCMが扱う整数アイテムによるトランザクションファイルを生成する。
		# この時、pos,negを併合して一つのファイルとして作成され(@wNumTraFile)、
		# 重みファイル(@weightFile[クラス])の作成は以下の通り。
		# 1.対象クラスをpos、その他のクラスをnegとする。
		# 2. negの重みは-1に設定し、posの重みはcalOmegaで計算した値。
		# 3.@wNumTraFileの各行のクラスに対応した重みデータを出力する(１項目のみのデータ)。
		self.weightFile = {}
		self.posWeight  = {}
		self.sigma      = {}

		for cName,posSize in db.clsNameRecSize.items(): 
			self.weightFile[cName] = self.temp.file()
			self.posWeight[cName]  = self.calOmega(posSize)
			cpara = "%s:%s"%(cName,self.posWeight[cName])
			nm.mcut(nfno=True,f=self.db.clsFN,i=self.db.cFile).mchgstr(nfn=True,f=0,O=-1,o=self.weightFile[cName],c=cpara).run()


		# アイテムをシンボルから番号に変換する。
		f=None
		f <<= nm.mjoin(k=self.db.itemFN,K=items.itemFN,m=items.file,f=items.idFN,i=self.db.file)
		f <<= nm.mcut(f=self.db.idFN+","+self.db.timeFN+","+items.idFN)
		f <<= nm.msortf(f=self.db.idFN+","+self.db.timeFN+"%n")
		f <<= nm.mtra(k=self.db.idFN,f=items.idFN)
		f <<= nm.mcut(f=items.idFN,nfno=True,o=self.file)
		f.run()

	def enumerate(self,eArgs):
		tf=nu.Mtemp()

		# 最小サポートと最小サポート件数
		if "minCnt" in eArgs :
			self.minCnt = int(eArgs["minCnt"])
			self.minSup = float(self.minCnt)/ float(self.db.size)
		else:
			self.minSup = float(eArgs["minSup"])
			self.minCnt = int(self.minSup * float(self.db.size) + 0.99)


		# 最大サポートと最大サポート件数
		self.maxCnt=None
		if "maxCnt" in eArgs or "maxSup" in eArgs:
			if "maxCnt" in eArgs:
				self.maxCnt = int(eArgs["maxCnt"])
				self.maxSup = float(self.maxCnt)/float(self.db.size)
			else:
				self.maxSup = float(eArgs["maxSup"])
				self.maxCnt = int(self.maxSup * float(self.db.size) + 0.99)

		#未使用
		#@minProb = eArgs["minProb"].to_f # 事後確率
		#@minGR   = @minProb/(1-@minProb) # 増加率
		#@minGR   = eArgs["minGR"].to_f if eArgs["minGR"]

		# あるクラスをpos、他のクラスをnegにして、パターン列挙した結果ファイル名を格納する
		pFiles=[]
		tFiles=[]
		for cName,posSize in self.db.clsNameRecSize.items(): 
			negSize=self.db.size-posSize

			# minGRの計算
			if "minGR" in eArgs:
				self.minGR=eArgs["minGR"]
			else:
				minProb = eArgs["minProb"] if ( "minProb" in eArgs ) else 0.5
				if "uniform" in eArgs and eArgs["uniform"]:
					self.minGR = (minProb/(1-minProb)) * (self.db.clsSize-1) # マニュアルの式(4)
				else:
					self.minGR = (minProb/(1-minProb)) * (float(negSize)/float(posSize)) # マニュアルの式(4)


			# 最小サポートと最小サポート件数
			if "minCnt" in eArgs:
				self.minPos = eArgs["minCnt"]
			else:
				self.minPos = int(eArgs["minSup"] * float(posSize) + 0.99)

			# 最大サポートと最大サポート件数
			if "maxCnt" in eArgs or "maxSup" in eArgs:
				if "maxCnt" in eArgs:
					self.maxCnt = int(eArgs["maxCnt"])
				else:
 					self.maxCnt = int(eArgs["maxSup"] * float(posSize) + 0.99)


			self.sigma[cName] = self.calSigma(self.minPos,self.minGR,posSize,negSize)

			# lcm_seqのパラメータ設定と実行
			lcmout = tf.file() # lcm_seq出力ファイル
			# 頻出パターンがなかった場合、lcm出力ファイルが生成されないので
			# そのときのために空ファイルを生成しておいく。
			with open(lcmout, "w") as efile:
				pass

			params = {}
			if self.msgoff:
				params["type"] ="CIA_"
			else:
				params["type"] ="CIA"

			if self.maxCnt: # windowサイズ上限
				params["U"] = str(self.maxCnt)
			if "minLen" in eArgs:
				params["l"] = str(eArgs["minLen"])
			if 'maxLen' in eArgs:
				params["u"] = str(eArgs["maxLen"])
			if 'gap' in eArgs:
				params["g"] = str(eArgs["gap"])
			if 'win' in eArgs:
				params["G"] = str(eArgs["win"])

			params["w"] = self.weightFile[cName]
			params["i"] = self.file
			params["sup"] = str(self.sigma[cName])
			params["o"] = lcmout

			# lcm_seq実行
			#MCMD::msgLog("#{run}")
			if 'padding' in eArgs and eArgs["padding"]: # padding指定時は、0アイテムを出力しないlcm_seqを実行
				extTake.lcmseq_zero(params)
			else:
				extTake.lcmseq(params)

			# パターンのサポートを計算しCSV出力する
			#MCMD::msgLog("output patterns to CSV file ...")
			pFiles.append(self.temp.file())
			transle = self.temp.file()

			extTake.lcmtrans(lcmout,"e",transle) # pattern,countP,countN,size,pid

			f=None
			f <<= nm.mdelnull(f="pattern",i=transle)
			f <<= nm.mcal(c='round(${countN},1)',a="neg")
			f <<= nm.mcal(c='round(${countP}/%s,1)'%(self.posWeight[cName]),a="pos")
			f <<= nm.mdelnull(f="pattern")
			f <<= nm.msetstr(v=cName,a="class")
			f <<= nm.msetstr(v=posSize,a="posTotal")
			f <<= nm.msetstr(v=self.minGR,a="minGR")
			f <<= nm.mcut(f="class,pid,pattern,size,pos,neg,posTotal,minGR",o=pFiles[-1])
			f.run()

			#s = MCMD::mrecount("i=#{pFiles.last}") # 列挙されたパターンの数
			#MCMD::msgLog("the number of contrast patterns on class `#{cName}' enumerated is #{s}")

			if self.outtf :
				# トランザクション毎に出現するシーケンスを書き出す
				#MCMD::msgLog("output tid-patterns ...")
				tFiles.append(self.temp.file())

				xxw= tf.file()
				f=None
				f <<= nm.mcut(f=self.db.idFN,i=self.db.file)
				f <<= nm.muniq(k=self.db.idFN)
				f <<= nm.mnumber(S=0,a="__tid",q=True)
				f <<= nm.msortf(f="__tid",o=xxw)
				f.run()

				nm.mcut(f=self.db.idFN,i=self.db.file).muniq(k=self.db.idFN).mnumber(S=0,a="__tid",q=True,o=xxw).run()
				translt = self.temp.file()
				extTake.lcmtrans(lcmout,"t",translt)
				nm.mjoin(k="__tid",m=xxw,f=self.db.idFN,i=translt).msetstr(v=cName,a="class").mcut(f=self.db.idFN+",class,pid",o=tFiles[-1]).run()


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
		f=""
		f =   nm.mcut(f="class,ppid:pid,pattern,size,pos,neg,posTotal,minGR",i=xxpCat)
		f <<= nm.msetstr(v=self.db.size,a="total")
		f <<= nm.mcal(c='${total}-${posTotal}',a="negTotal") # negのトータル件数
		f <<= nm.mcal(c='${pos}/${posTotal}',a="support") # サポートの計算
		f <<= nm.mcal(c='if(${neg}==0,1.797693135e+308,(${pos}/${posTotal})/(${neg}/${negTotal}))',a="growthRate")
		if "uniform" in eArgs and eArgs["uniform"] == True:
			f <<= nm.mcal(c='(${pos}/${posTotal})/(${pos}/${posTotal}+(%s-1)*${neg}/${negTotal})'%(self.db.clsSize),a="postProb")
		else:
			f <<= nm.mcal(c='${pos}/(${pos}+${neg})',a="postProb")

		f <<= nm.msel(c='${pos}>=%s&&${growthRate}>=${minGR}'%(self.minPos)) # minSupとminGRによる選択
		f <<= nm.mvreplace(vf="pattern",m=items.file,K=items.idFN,f=items.itemFN)
		f <<= nm.mcut(f="class,pid,pattern,size,pos,neg,posTotal,negTotal,total,support,growthRate,postProb")
		f <<= nm.mvsort(vf="pattern")
		f <<= nm.msortf(f="class%nr,postProb%nr,pos%nr",o=self.pFile)
		f.run()

		if self.outtf :
			# 列挙されたパターンを含むtraのみ選択するためのマスタ
			xxp4=nm.mcut(f="class,pid",i=self.pFile)

			f =   nm.mcat(i=",".join(tFiles))
			f <<= nm.mjoin(k="class,pid",m=xxpCat,f="ppid") # 全クラス統一pid(ppid)結合
			f <<= nm.mcommon(k="class,ppid",K="class,pid",m=xxp4) # 列挙されたパターンの選択
			f <<= nm.mcut(f=self.db.idFN+",class,ppid:pid")
			f <<= nm.msortf(f=self.db.idFN+",class,pid",o=self.tFile)
			f.run()


		self.size = nu.mrecount(i=self.pFile)
		#MCMD::msgLog("the number of emerging sequence patterns enumerated is #{@size}")

	def output(self,outpath):
		shutil.move(self.pFile,outpath+"/patterns.csv")
		if self.outtf:
			shutil.move(self.tFile,outpath+"/tid_pats.csv")


