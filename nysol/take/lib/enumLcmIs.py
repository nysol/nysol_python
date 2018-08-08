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
import nysol.util.mtemp as mtemp
import nysol.util.mrecount as mrecount
import nysol.mcmd as nm
from nysol.take import extcore as extTake
import nysol.vsop._vsoplib as VSOP

#========================================================================
# 列挙関数:lcm 利用DB:TraDB
#========================================================================
class LcmIs(object):
	"""
	:type db: nysol.take.traDB.TraDBオブジェクト
	:param db: transactionデータベース名

	トランザクションデータベースから頻出アイテム集合を列挙するクラス。

	例1) 最小サポート件数が3件、最小アイテムサイズが2件の頻出アイテム集合を列挙し、その後に同条件の頻出飽和集合を求める。

	.. code-block:: python
		:linenos:

		import os
		from nysol.take.traDB import TraDB
		from nysol.take.enumLcmIs import LcmIs

		d='''id,item
		"T1","C"
		"T1","E"
		"T2","D"
		"T2","E"
		"T2","F"
		"T3","A"
		"T3","B"
		"T3","D"
		"T3","A"
		"T4","B"
		"T4","D"
		"T4","F"
		"T5","A"
		"T5","B"
		"T5","D"
		"T5","E"
		"T6","A"
		"T6","B"
		"T6","D"
		"T6","E"
		"T6","F"
		'''

		with open("dat1.csv","w") as fpw:
	  		fpw.write(d)

				db=TraDB(iFile="dat1.csv",idFN="id",itemFN="item")

				lcm=LcmIs(db)

		eArgs={}
		eArgs["type"]="F"
		eArgs["minCnt"]=3
		eArgs["minLen"]=2
		lcm.enumerate(eArgs)
		oPath="./xxrsl1"
		os.system("mkdir %s"%oPath)
		lcm.output(oPath)
		print(lcm)

	.. code-block:: python
		:linenos:

		## class name: nysol.take.enumLcmIs.LcmIs
		id: 10805b4a8
		eArgs: {'type': 'F', 'minCnt': 3, 'minLen': 2}
		minCnt: 3
		minSup: 0.5
		maxCnt: None
		maxSup: None
		minLen: None
		maxLen: None
		top: None
		skipTP: False

		## class name: nysol.take.enumLcmIs.LcmIs
		id: 10805b4a8
		eArgs: {'type': 'C', 'minCnt': 3, 'minLen': 2}
		minCnt: 3
		minSup: 0.5
		maxCnt: None
		maxSup: None
		minLen: None
		maxLen: None
		top: None
		skipTP: False
	"""
	def __init__(self,db):
		self.db=db
		self.eArgs=None
		self.type =None
		self.minCnt=None
		self.minSup=None
		self.maxCnt=None
		self.maxSup=None
		self.minLen=None
		self.maxLen=None
		self.top =None
		self.skipTP=False

		#self.size =None
		self.pFile =None
		self.tFile =None
		self.msgoff = True

		self.temp=mtemp.Mtemp()
		self.db = db # 入力データベース
		self.file=self.temp.file()
		items=self.db.items

		# アイテムをシンボルから番号に変換する。
		f =   nm.mjoin(k=self.db.itemFN,K=items.itemFN,m=items.file,f=items.idFN,i=self.db.file)
		f <<= nm.mcut(f=self.db.idFN+","+items.idFN)
		f <<= nm.mtra(k=self.db.idFN,f=items.idFN)
		f <<= nm.mcut(f=items.idFN,nfno=True,o=self.file)
		f.run()

	def reduceTaxo(self,pat,items):
		#tf=MCMD::Mtemp.new

		if items.taxonomy==None:
			return pat

		taxo=items.taxonomy
		xxrt = nm.mtrafld(f=taxo.itemFN+","+taxo.taxoFN,valOnly=True,a="__fld",i=taxo.file).mcut(f="__fld")
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

		#print("-------")
		#pat.Print()
		pat=pat.restrict(oyako).iif(0,pat)
		#print("-------")
		#pat.Print()

		return pat

	def __str__(self):
		ret ="## class name: "+self.__module__+"."+self.__class__.__name__+"\n"
		ret+="id: "+"%x"%id(self)+"\n"
		ret+="eArgs: "+self.eArgs.__str__()+"\n"
		ret+="minCnt: "+str(self.minCnt)+"\n"
		ret+="minSup: "+str(self.minSup)+"\n"
		ret+="maxCnt: "+str(self.maxCnt)+"\n"
		ret+="maxSup: "+str(self.maxSup)+"\n"
		ret+="minLen: "+str(self.minLen)+"\n"
		ret+="maxLen: "+str(self.maxLen)+"\n"
		ret+="top: "   +str(self.top)+"\n"
		ret+="skipTP: "+str(self.skipTP)+"\n"
		#ret+="db: "+self.db.__str__()+"\n"
		#ret+="size: "  +str(self.size)+"\n"
		return ret

	def enumerate(self,eArgs):
		"""
		eArgsで与えられた条件で、頻出アイテム集合の列挙を実行する。

		:type eArgs: dict
		:type eArgs['type']: str
		:type eArgs['minCnt']: int
		:type eArgs['minSup']: float
		:type eArgs['maxCnt']: int
		:type eArgs['maxSup']: float
		:type eArgs['minLen']: int
		:type eArgs['maxLen']: int
		:type eArgs['top']: int
		:type eArgs['skipTP']: bool【default:False】
		:param eArgs: 各種列挙パラメータ
		:param eArgs['type']: 抽出するアイテム集合の型【'F':頻出集合, 'C':飽和集合, 'M':極大集合】
		:param eArgs['minCnt']: 最小サポート(件数)
		:param eArgs['minSup']: 最小サポート(確率)
		:param eArgs['maxCnt']: 最大サポート(件数)
		:param eArgs['maxSup']: 最大サポート(確率)
		:param eArgs['minLen']: アイテム集合の最小アイテム数(件数)
		:param eArgs['maxLen']: アイテム集合の最大アイテム数(件数)
		:param eArgs['top']: 列挙するサポート上位件数(件数)
		:param eArgs['skipTP']: トランザクションにマッチするパターン(アイテム集合)の出力を行わない。
		"""

		tf=mtemp.Mtemp()
		self.eArgs=eArgs
		self.type = eArgs["type"]

		if "minCnt" in eArgs and eArgs["minCnt"] != None:
			self.minCnt = int(eArgs["minCnt"])
			self.minSup = float(self.minCnt) / float(self.db.traSize)
		else:
			self.minSup = float(eArgs["minSup"])
			self.minCnt = int(self.minSup * float(self.db.traSize) + 0.99)

		# 最大サポートと最大サポート件数
		self.maxCnt=None
		if ("maxCnt" in eArgs and  eArgs["maxCnt"]!= None) or ( "maxSup" in eArgs and eArgs["maxSup"]!= None):
			if "maxCnt" in eArgs and eArgs["maxCnt"]!= None:
				self.maxCnt = int(eArgs["maxCnt"])
				self.maxSup = float(self.maxCnt) / float(self.db.traSize)
			else:
				self.maxSup    = float(eArgs["maxSup"])
				self.maxCnt = int(self.maxSup * float(self.db.traSize) + 0.99)


		params = {}
		if self.msgoff:
			params["type"] ="%sIf_"%(self.type)
		else:
			params["type"] ="%sIf"%(self.type)


		if self.maxCnt :
			params["U"] = str(self.maxCnt)

		if "minLen" in eArgs and eArgs["minLen"] != None :
			params["l"] = str(eArgs['minLen'])
		
		if "maxLen" in eArgs and eArgs["maxLen"] != None :
			params["u"] = str(eArgs['maxLen'])

		# 列挙パターン数上限が指定されれば、一度lcmを実行して最小サポートを得る
		if "top" in eArgs and eArgs["top"] != None :
			self.top = eArgs["top"]

		if self.top and self.top>0 :

			xxtop = tf.file()
			import copy
			top_params = copy.deepcopy(params)
			top_params["i"] = self.file
			top_params["sup"] = "1"
			top_params["K"] = str(self.top)
			top_params["so"] = xxtop
			import re
			top_params["type"] = re.sub('_$', '', top_params["type"] )

			extTake.lcm(top_params)

			with open(xxtop, "r") as rfile:
				self.minCnt = int(rfile.read().strip())

			if self.minCnt<0 :
				self.minCnt=1 


		self.skipTP=False
		if "skipTP" in eArgs:
			self.skipTP=eArgs["skipTP"]

		# lcm_seq出力ファイル
		lcmout = tf.file()

		# 頻出パターンがなかった場合、lcm出力ファイルが生成されないので
		# そのときのために空ファイルを生成しておいく。
		with open(lcmout, "w") as efile:
			pass

		# lcm実行
		params["i"] = self.file
		params["sup"] = str(self.minCnt)
		params["o"] = lcmout
		extTake.lcm(params)

		# caliculate one itemset for lift value
		xxone= tf.file()
		tpstr = "FIf_" if self.msgoff else "FIf"

		extTake.lcm(type=tpstr,i=self.file,sup=1,o=xxone,l=1,u=1)


		# パターンのサポートを計算しCSV出力する
		#MCMD::msgLog("output patterns to CSV file ...")

		xxp0 = tf.file()
		self.pFile = self.temp.file()
		items=self.db.items
		trans0 = self.temp.file()

		extTake.lcmtrans(lcmout,"p",trans0)

		f =   nm.mdelnull(i=trans0,f="pattern")
		f <<= nm.mvreplace(vf="pattern",m=items.file,K=items.idFN,f=items.itemFN)
		f <<= nm.msetstr(v=self.db.traSize,a="total")
		f <<= nm.mcal(c='${count}/${total}',a="support")
		f <<= nm.mcut(f="pid,pattern,size,count,total,support")
		f <<= nm.mvsort(vf="pattern")
		f <<= nm.msortf(f="pid",o=xxp0)
		f.run()


		# xxp0
		# pid,count,total,support,pattern
		# 0,13,13,1,A
		# 4,6,13,0.4615384615,A B
		xxp1=tf.file()

		# taxonomy指定がない場合(2010/11/20追加)
		if items.taxonomy==None:
			shutil.move(xxp0,xxp1)
		# taxonomy指定がある場合
		else:
			#MCMD::msgLog("reducing redundant rules in terms of taxonomy ...")

			zdd=VSOP.constant(0)
			fobj = nm.mcut(i=xxp0,f='pattern')
			for fldVal in fobj:
				zdd=zdd+VSOP.itemset(fldVal[0])

			
			zdd=self.reduceTaxo(zdd,self.db.items)
			xxz1=tf.file()
			xxz2=tf.file()
			zdd.csvout(xxz1)

			f0=None
			f0 <<= nm.mcut(nfni=True,f="1:pattern",i=xxz1)
			f0 <<= nm.mvsort(vf="pattern")
			f0 <<= nm.msortf(f="pattern")

			f=None
			f <<= nm.msortf(f="pattern",i=xxp0)
			f <<= nm.mcommon(k="pattern",m=f0)
			f <<= nm.msortf(f="pid",o=xxp1)
			f.run()


		# lift値の計算		
		transl = tf.file()
		extTake.lcmtrans(xxone,"p",transl)

		xxp2 =   nm.mdelnull(i=transl,f="pattern")
		xxp2 <<= nm.mvreplace(vf="pattern",m=items.file,K=items.idFN,f=items.itemFN)
		xxp2 <<= nm.msortf(f="pattern")

		xxp3 =   nm.mcut(f="pid,pattern",i=xxp1)
		xxp3 <<= nm.mtra(f="pattern",r=True)
		xxp3 <<= nm.mjoin(k="pattern",m=xxp2,f="count:c1")
		xxp3 <<= nm.mcal(c='ln(${c1})',a="c1ln")
		xxp3 <<= nm.msum(k="pid",f="c1ln")

		# p3
		# pid,pattern,c1,c1ln
		# 0,A,13,2.564949357
		# 1,E,7,1.945910149
		
		#おかしくなる?=>OK
		f3 =   nm.mjoin(k="pid",f="c1ln",i=xxp1,m=xxp3)
		f3 <<= nm.mcal(c='round(exp(ln(${count})-${c1ln}+(${size}-1)*ln(${total})),0.0001)',a="lift")
		f3 <<= nm.mcut(f="pid,size,count,total,support,lift,pattern")
		f3 <<= nm.msortf(f="support%nr",o=self.pFile)
		f3.run()

		#self.size = mrecount.mrecount(i=self.file)

		#MCMD::msgLog("the number of patterns enumerated is #{@size}")

		if not self.skipTP:
			# トランザクション毎に出現するシーケンスを書き出す
			#MCMD::msgLog("output tid-patterns ...")

			self.tFile = self.temp.file()
			xxw3i = tf.file()
			extTake.lcmtrans(lcmout,"t",xxw3i)

			xxw1 = nm.mcut(f=self.db.idFN,i=self.db.file).muniq(k=self.db.idFN).mnumber(S=0,a="__tid",q=True).msortf(f="__tid")
			xxw2 = nm.mcut(f="pid",i=self.pFile)
			xxw3 = nm.mcommon(k="pid",i=xxw3i,m=xxw2).mjoin(k="__tid",m=xxw1,f=self.db.idFN).mcut(f=self.db.idFN+",pid",o=self.tFile)
			xxw3.run()

	def output(self,outpath):
		"""
		列挙された結果を指定のパス(outpath)に保存する。

		:type outpath: str
		:param outpath: 結果を出力するパス名。存在しなければエラーとなるので、事前にディレクトリを作成しておくこと。
		"""
		shutil.move(self.pFile,outpath+"/patterns.csv")
		if not self.skipTP:
			shutil.move(self.tFile,outpath+"/tid_pats.csv")
  		
