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
#require "nysol/seqDB.rb"
import os
import shutil
import nysol.util.mtemp as mtemp
import nysol.mcmd as nm
from nysol.take import extcore as extTake


#========================================================================
# 列挙関数:lcmseq 利用DB:SeqDB
#========================================================================
class LcmSeq(object):

	def __init__(self,db,outtf=True):
		self.size =None
		self.temp=mtemp.Mtemp()
		self.db = db # 入力データベース
		self.file=self.temp.file()
		items=self.db.items
		self.outtf = outtf
		self.top =None
		self.msgoff = True
		# アイテムをシンボルから番号に変換する。
		f = nm.mjoin(k=self.db.itemFN , K=items.itemFN, m=items.file,f=items.idFN,i=self.db.file)
		f <<= nm.mcut(f=self.db.idFN+","+self.db.timeFN+","+items.idFN)
		f <<= nm.mtra(k=self.db.idFN,s=self.db.timeFN+"%n",f=items.idFN)
		f <<= nm.mcut(f=items.idFN,nfno=True,o=self.file)
		f.run()


	def enumerate(self,eArgs):
		tf=mtemp.Mtemp()

		# 最小サポートと最小サポート件数
		if "minCnt" in eArgs and eArgs["minCnt"] != None:
			self.minCnt = int(eArgs["minCnt"])
			self.minSup = float(self.minCnt)/float(self.db.size)
		else:
			self.minSup =float(eArgs["minSup"])
			self.minCnt =int(self.minSup * float(self.db.size) + 0.99)


		# 最大サポートと最大サポート件数
		self.maxCnt=None
		if ("maxCnt" in eArgs and  eArgs["maxCnt"]!= None) or ( "maxSup" in eArgs and eArgs["maxSup"]!= None):
			if "maxCnt" in eArgs and eArgs["maxCnt"]!= None:
				self.maxCnt = int(eArgs["maxCnt"])
				self.maxSup = float(self.maxCnt)/float(self.db.size)
			else:
				self.maxSup = float(eArgs["maxSup"])
				self.maxCnt = int(self.maxSup * float(self.db.size)+ 0.99)


		# 列挙パターン数上限が指定されれば、一度lcmを実行して最小サポートを得る
		if "top" in eArgs and eArgs["top"] != None :
			self.top = eArgs["top"]

		# 列挙パターン数上限が指定されれば、一度lcmを実行して最小サポートを得る
		if self.top and self.top>0 :

			xxtop = tf.file()
			 
			extTake.lcmseq(type="Cf",K=str(self.top),i=self.file,sup="1" ,so=xxtop)

			with open(xxtop, "r") as rfile:
				self.minCnt = int(rfile.read().strip())


		# lcm_seq出力ファイル
		lcmout = tf.file()
		# 頻出パターンがなかった場合、lcm出力ファイルが生成されないので
		# そのときのために空ファイルを生成しておいく。
		with open(lcmout, "w") as efile:
			pass

		# lcm_seqのパラメータ設定と実行
		params = {}
		if self.msgoff:
			params["type"] ="CIf_"
		else:
			params["type"] ="CIf"

		if self.maxCnt :
			params["U"] = str(self.maxCnt)
		if "minLen" in eArgs:
			params["l"] = str(eArgs["minLen"])
		if 'maxLen' in eArgs:
			params["u"] = str(eArgs["maxLen"])
		if 'gap' in eArgs:
			params["g"] = str(eArgs["gap"])
		if 'win' in eArgs:
			params["G"] = str(eArgs["win"])
		
		params["i"] = self.file
		params["sup"] = str(self.minCnt)
		params["o"] = lcmout


		# lcm_seq実行
		#MCMD::msgLog("#{run}")
		if 'padding' in eArgs and eArgs["padding"] : # padding指定時は、0アイテムを出力しないlcm_seqを実行
			extTake.lcmseq_zero(params)
		else:
			extTake.lcmseq(params)

		# パターンのサポートを計算しCSV出力する
		self.pFile = self.temp.file()
		items=self.db.items

		transl = self.temp.file()
		extTake.lcmtrans(lcmout,"p",transl)


		f = nm.mdelnull(f="pattern",i=transl)
		f <<= nm.mvreplace(vf="pattern",m=items.file,K=items.idFN,f=items.itemFN)
		f <<= nm.msetstr(v=self.db.size, a="total")
		f <<= nm.mcal(c='${count}/${total}',a="support") # サポートの計算
		f <<= nm.mcut(f="pid,pattern,size,count,total,support")
		f <<= nm.msortf(f="support%nr",o=self.pFile)
		f.run()


		if self.outtf :
			# トランザクション毎に出現するシーケンスを書き出す
			#MCMD::msgLog("output tid-patterns ...")
			self.tFile = self.temp.file()

			xxw = tf.file() #Mtemp.new.name
			f=None
			f <<= nm.mcut(f=self.db.idFN,i=self.db.file)
			f <<= nm.muniq(k=self.db.idFN)
			f <<= nm.mnumber(S=0,a="__tid",q=True)
			f <<= nm.msortf(f="__tid",o=xxw)
			f.run()

			translt = self.temp.file()
			extTake.lcmtrans(lcmout,"t",translt)

			f=None
			f <<= nm.msortf(f="__tid",i=translt)
			f <<= nm.mjoin(k="__tid",m=xxw,f=self.db.idFN)
			f <<= nm.mcut(f=self.db.idFN+",pid")
			f <<= nm.msortf(f=self.db.idFN+",pid",o=self.tFile)
			f.run()

		#	self.size = nutil.mrecount(i=self.pFile) # 列挙されたパターンの数
		#MCMD::msgLog("the number of contrast patterns enumerated is #{@size}")

	def output(self,outpath):
		shutil.move(self.pFile,outpath+"/patterns.csv")
		if self.outtf:
			shutil.move(self.tFile,outpath+"/tid_pats.csv")


