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

#require "rubygems"
#require "nysol/mcmd"
#require "nysol/zdd"
#require "nysol/take"
#require "nysol/traDB.rb"
import os
import shutil
import mtemp as nutil
import nysol.mod as nm
import nysol.take._lcmlib as ntlcm
import nysol.take._lcmtranslib as ntrans

#========================================================================
# 列挙関数:lcm 利用DB:TraDB
#========================================================================
class LcmIs(object):

	"""
	未実装 ZDD作成してから
	def reduceTaxo(pat,items)
		tf=MCMD::Mtemp.new

		if items.taxonomy==nil then
			return pat
		end

		xxrt = tf.file
		taxo=items.taxonomy
		f=""
		f << "mtrafld f=#{taxo.itemFN},#{taxo.taxoFN} -valOnly a=__fld i=#{taxo.file} o=#{xxrt}"
		system(f)

		# xxrtの内容：oyakoに親子関係にあるアイテム集合のリストが格納される
		# __fld
		# A X
		# B X
		# C Y
		# D Z
		# E Z
		# F Z
		oyako=ZDD.constant(0)
		MCMD::Mcsvin.new("i=#{xxrt}"){|csv|
			csv.each{|fldVal|
				items=fldVal["__fld"]
				oyako=oyako+ZDD.itemset(items)
			}
		}

		# 親子リストにあるアイテム集合を含むパターンを削除する
		pat=pat.restrict(oyako).iif(0,pat)

		return pat
	end
	"""

	def __init__(self,db,outtf=True):

		self.size =None
		self.pFile =None
		self.tFile =None
		self.type =None
		self.top =None

		self.temp=nutil.Mtemp()
		self.db = db # 入力データベース
		self.file=self.temp.file()
		self.items=self.db.items
		self.outtf = outtf

		# アイテムをシンボルから番号に変換する。
		f =   nm.mjoin(k=self.db.itemFN,K=self.items.itemFN,m=self.items.file,f=self.items.idFN,i=self.db.file)
		f <<= nm.mcut(f=self.db.idFN+","+self.items.idFN)
		f <<= nm.mtra(k=self.db.idFN,f=self.items.idFN)
		f <<= nm.mcut(f=self.items.idFN,nfno=True,o=self.file)
		f.run()

	def enumerate(self,eArgs):

		tf=nutil.Mtemp()
		self.type = eArgs["type"]

		if "minCnt" in eArgs and eArgs["minCnt"] != None:
			self.minCnt = int(eArgs["minCnt"])
			self.minSup = float(self.minCnt) / float(self.db.size)
		else:
			self.minSup = float(eArgs["minSup"])
			self.minCnt = int(self.minSup * float(self.db.size) + 0.99)

		# 最大サポートと最大サポート件数
		self.maxCnt=None
		if ("maxCnt" in eArgs and  eArgs["maxCnt"]!= None) or ( "maxSup" in eArgs and eArgs["maxSup"]!= None):
			if "maxCnt" in eArgs and eArgs["maxCnt"]!= None:
				self.maxCnt = int(eArgs["maxCnt"])
				self.maxSup = float(self.maxCnt) / float(self.db.size)
			else:
				self.maxSup    = float(eArgs["maxSup"])
				self.maxCnt = int(self.maxSup * float(self.db.size) + 0.99)


		params = {}
		params["type"] = "%sIf"%(self.type)

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

			#仮
			#TAKE::run_lcmK("#{run} -K #{@top} #{@file} 1 #{xxtop}")			
			#system("#{run} -K #{@top} #{@file} 1 > #{xxtop}")
			#os.system("lcm %s -K %s %s 1 > %s"%(run,self.top,self.file,xxtop))
			ntlcm.lcm_runByDict(top_params)

			with open(xxtop, "r") as rfile:
				self.minCnt = int(rfile.read().strip())

			if self.minCnt<0 :
				self.minCnt=1 


		# lcm_seq出力ファイル
		lcmout = tf.file()

		# 頻出パターンがなかった場合、lcm出力ファイルが生成されないので
		# そのときのために空ファイルを生成しておいく。
		with open(lcmout, "w") as efile:
			pass



		# lcm実行
		#MCMD::msgLog("#{run} #{@file} #{@minCnt} #{lcmout}")
		#print(run,self.file,self.minCnt,lcmout)
		#print("lcm %s %s %s %s"%(run,self.file,self.minCnt,lcmout))
		#TAKE::run_lcm("#{run} #{@file} #{@minCnt} #{lcmout}")
		#os.system("lcm %s %s %s %s"%(run,self.file,self.minCnt,lcmout))
		#ntlcm.lcm_run(type=run,i=self.file,sup=str(self.minCnt),o=lcmout)

		params["i"] = self.file
		params["sup"] = str(self.minCnt)
		params["o"] = lcmout
		print(params)
		ntlcm.lcm_runByDict(params)

		# caliculate one itemset for lift value
		xxone= tf.file()
		#TAKE::run_lcm("FIf -l 1 -u 1 #{@file} 1 #{xxone}")
		#os.system("lcm FIf -l 1 -u 1 %s 1 %s"%(,))
		ntlcm.lcm_run(type="FIf",i=self.file,sup="1",o=xxone,l="1",u="1")


		# パターンのサポートを計算しCSV出力する
		#MCMD::msgLog("output patterns to CSV file ...")

		xxp0 = tf.file()
		self.pFile = self.temp.file()
		items=self.db.items
		trans0 = self.temp.file()

		#TAKE::run_lcmtrans(lcmout,"p",trans0)
		#os.system("lcm_trans "+lcmout+" p > "+trans0)
		#os.system("cp "+lcmout + " xlogx")
		#os.system("cp "+self.items.file + " xmstx")
		#f = nm.mdelnull(i=trans0, f="pattern").mvreplace(vf="pattern",m=self.items.file,K=self.items.idFN,f=self.items.itemFN).msetstr(v=self.db.size,a="total").mcal(c='${count}/${total}',a="support").mcut(f="pid,pattern,size,count,total,support").mvsort(vf="pattern").msortf(f="pid",o=xxp0)
		#f.drawModelD3("xx.html")

		ntrans.lcmtrans_run(lcmout,"p",trans0)
		f =   nm.mdelnull(i=trans0,f="pattern")
		f <<= nm.mvreplace(vf="pattern",m=self.items.file,K=self.items.idFN,f=self.items.itemFN)
		f <<= nm.msetstr(v=self.db.size,a="total")
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
			#未実装
			"""
			zdd=ZDD.constant(0)
			MCMD::Mcsvin.new("i=#{xxp0}"){|csv|
				csv.each{|fldVal|
					pat=fldVal['pattern']
					zdd=zdd+ZDD.itemset(pat)
				}
			}

			zdd=reduceTaxo(zdd,@db.items)
			xxz1=tf.file
			xxz2=tf.file
			zdd.csvout(xxz1)
			f=""
			f << "mcut   -nfni f=1:pattern i=#{xxz1} |"
			f << "mvsort vf=pattern        |"
			f << "msortf f=pattern         o=#{xxz2}"
			system(f)

			f=""
			f << "msortf  f=pattern           i=#{xxp0} |"
			f << "mcommon k=pattern m=#{xxz2} |"
			f << "msortf  f=pid               o=#{xxp1}"
			system(f)
			"""


		# lift値の計算		
		#TAKE::run_lcmtrans(xxone,"p",transl)
		#os.system("lcm_trans "+xxone+" p > " + xxp2l)

		transl = tf.file()
		ntrans.lcmtrans_run(xxone,"p",transl)

		xxp2 =   nm.mdelnull(i=transl,f="pattern")
		xxp2 <<= nm.mvreplace(vf="pattern",m=self.items.file,K=self.items.idFN,f=self.items.itemFN)
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

		self.size = nutil.mrecount(i=self.file)


		#MCMD::msgLog("the number of patterns enumerated is #{@size}")

		if self.outtf:
			# トランザクション毎に出現するシーケンスを書き出す
			#MCMD::msgLog("output tid-patterns ...")
			#os.system("lcm_trans "+lcmout+" t > " + xxw3i)

			self.tFile = self.temp.file()
			xxw3i = tf.file()
			ntrans.lcmtrans_run(lcmout,"t",xxw3i)

			xxw1 = nm.mcut(f=self.db.idFN,i=self.db.file).muniq(k=self.db.idFN).mnumber(S=0,a="__tid",q=True).msortf(f="__tid")
			xxw2 = nm.mcut(f="pid",i=self.pFile)
			xxw3 = nm.mcommon(k="pid",i=xxw3i,m=xxw2).mjoin(k="__tid",m=xxw1,f=self.db.idFN).mcut(f=self.db.idFN+",pid",o=self.tFile)
			xxw3.run()


	def output(self,outpath):
		shutil.move(self.pFile,outpath+"/patterns.csv")
		if self.outtf :
			shutil.move(self.tFile,outpath+"/tid_pats.csv")
  		
