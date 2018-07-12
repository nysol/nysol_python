#!/usr/bin/env python
# -*- coding: utf-8 -*-

import shutil
import os
import nysol.mcmd as nm
import nysol.util as nu

#import nysol.util.mtemp as mtemp
#import nysol.util.mrecount as mrecount
from nysol.take.lib import items as items
#=トランザクションデータクラス
# 頻出パターンマイニングで使われるトランザクションデータを扱うクラス。
# トランザクションファイルは、トランザクションID項目とアイテム集合項目から構成される。
# アイテム集合は、スペースで区切られた文字列集合として表現される。
#
#===利用例
# 以下tra.csvの内容
# ---
# items
# a b d
# a d e
# c e
# a d
# c d e
# c e
# a d e
#
# 以下taxo.csvの内容
# ---
# item,taxo
# a,X1
# b,X1
# c,X2
# d,X2
# e,X2
#
# 以下rubyスクリプト
# ---
# require 'rubygems'
# require 'mining'
#
# tra=TraDB.new("tra.csv",nil,"items")
# puts tra.file       # => ./1252813069_2179/dat/1
# puts tra.numTraFile # => nil
# puts tra.idFN       # => tid
# puts tra.itemsetFN  # => items
# puts tra.recCnt     # => 7
# p    tra.items      # => #<Items:0x16a39ec @iItemFN=["items"],...
#
# taxo=Taxonomy.new("taxo.csv","item","taxo")
# tra.addTaxo(taxo)
# tra.mkNumTra
# puts tra.numTraFile #=> ./1252813069_2179/dat/6
# p    tra.items      #=> #<Items:0x16a39ec @iItemFN=["items", "taxo"],..., @taxonomy=#<Taxonomy:0x168ba7c ...
#
#=====./1252813069_2179/dat/1 の内容
# tid,items
# 1,a b d
# 2,a d e
# 3,c e
# 4,a d
# 5,c d e
# 6,c e
# 7,a d e
#
#=====./1252813069_2179/dat/6 の内容
# 1 2 3 4 5 6 7
# 1 2 4 6 7
# 1 4 5 6 7
# 3 5 7
# 1 4 6 7
# 3 4 5 7
# 3 5 7
# 1 4 5 6 7
#
class TraDB(object):


	#=== TraDBクラスの初期化
	#
	#====引数
	# iFile: transactionファイル名
	# iItemsetFN: iFile上のアイテム集合項目名
	# idFN: 新しく命名するトランザクションID項目名
	# itemsetFN: 新しく命名するアイテム集合項目名
	#
	#====機能
	#* トランザクションIDとアイテム集合の2項目から構成するトランザクションデータを新たに作成する。
	#* ID項目が指定されていなければ、1から始まる連番によってID項目を新規に作成する。
	#* トランザクション件数(iFileのレコード件数)を計算する。
	#* アイテム集合項目からItemsオブジェクトを生成する。
	def __init__(self,iFile,idFN,itemFN,clsFN=None):
		self.file     = None # トランザクションファイル名
		self.idFN     = None # トランザクションID項目名(String)
		self.itemFN   = None # アイテム集合項目名(String)
		self.clsFN    = None # クラス項目名(String)
		self.traSize  = None # トランザクションサイズ(Num)
		self.items    = None # Itemsクラス
		self.taxonomy = None # 階層分類クラス
		self.clsNameRecSize = None # クラス別件数
		self.clsSize  = None # クラス数
		self.cFile    = None # クラスファイル

		self.temp   = nu.Mtemp()
		self.iFile  = iFile                    # 入力ファイル
		self.iPath  = os.path.abspath(self.iFile) # フルパス
		self.idFN   = idFN                     # トランザクションID項目名
		self.itemFN = itemFN                   # 入力ファイル上のアイテム集合項目名
		self.file   = self.temp.file()               # 出力ファイル名
		
		fpara = self.idFN + "," + self.itemFN

		nm.mcut(f=fpara,i=self.iFile).muniq(k=fpara,o=self.file).run()

		# レコード数の計算
		self.recCnt = nu.mrecount(i=self.file)

		# トランザクション数の計算
		xx1 = nm.mcut(f=self.idFN,i=self.file).muniq(k=self.idFN).mcount(a='__cnt').mcut(f='__cnt').run()
		self.traSize = int(xx1[0][0])
		
		# トランザクションデータからアイテムオブジェクトを生成
		self.items=items.Items(self.file,self.itemFN)

		# クラスデータ
		if clsFN :
			self.clsFN=clsFN
			self.cFile=self.temp.file()
			fpara_c = "%s,%s"%(self.idFN,self.clsFN)

			nm.mcut(f=fpara_c,i=self.iFile).muniq(k=fpara_c,o=self.cFile).run()


			# 文字列としてのクラス別件数配列を数値配列に変換する
			self.clsSize=0
			self.clsNames = []
			self.clsNameRecSize = {}
			for vv in nm.mcut(f=self.clsFN,i=self.cFile).mcount(k=self.clsFN,a='count'):
				self.clsNames.append(vv[0])	 
				self.clsNameRecSize[vv[0]] = int(vv[1]) 
				self.clsSize+=1


	def replaceFile(self,train):

		self.file   = train

		# レコード数の計算
		self.recCnt = nu.mrecount(i=self.file)


		xx1 = nm.mcut(f=self.idFN,i=self.file).muniq(k=self.idFN).mcount(a='__cnt').mcut(a='__cnt').run()
		self.traSize = int(xx1[0][0])

	#=== taxonomyをトランザクションに追加
	# トランザクションデータのアイテム集合に、対応するtaxonomyを追加する。
	#
	#====引数
	# taxonomy: Taxonomyオブジェクト。
	#
	#====機能
	#* トランザクションデータのアイテム集合項目におけるアイテム全てについて、対応するtaxonomyをアイテム集合として追加する。
	def addTaxo(self,taxonomy):

		self.taxonomy=taxonomy

		self.items.addTaxo(self.taxonomy) # アイテムクラスにtaxonomyを追加する

		tFile =taxonomy.file
		itemFN=taxonomy.itemFN
		taxoFN=taxonomy.taxoFN

		tf=nu.Mtemp()
		xx1=tf.file()
		cpara = "%s,%s:%s"%(self.idFN,taxoFN,self.itemFN)
		f=None
		f <<= nm.mnjoin(i=self.file,k=self.itemFN,K=itemFN,f=taxoFN,m=tFile)
		f <<= nm.mcut(f=cpara,o=xx1)
		f.run()


		ipara ="%s,%s"%(self.file,xx1)
		kpara ="%s,%s"%(self.idFN,self.itemFN)
		xx2=tf.file()
		nm.mcat(i=ipara).muniq(k=kpara,o=xx2).run(msg="on")

		self.file = self.temp.file()
		shutil.move(xx2,self.file)



	def repTaxo(self,taxonomy):

		#@taxonomy=taxonomy #replaceの場合はtaxonomyを登録しない

		self.items.repTaxo(taxonomy) # アイテムクラスをtaxonomyで置換する

		tFile =taxonomy.file
		itemFN=taxonomy.itemFN
		taxoFN=taxonomy.taxoFN

		tf=nu.Mtemp()
		xx1=tf.file()
		cpara = "%s,%s:%s"%(self.idFN,taxoFN,self.itemFN)
		kpara = "%s,%s"%(self.idFN,self.itemFN)
		
		nm.mjoin(i=self.file,k=self.itemFN,K=itemFN,f=taxoFN,m=tFile).mcut(f=cpara).muniq(k=kpara,o=xx1).run()

		self.file = self.temp.file()
		shutil.move(xx1,self.file)

