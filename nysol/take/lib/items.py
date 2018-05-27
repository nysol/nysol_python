#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os

import shutil
from nysol.util.mtemp import Mtemp
from nysol.util.mrecount import mrecount

import nysol.mod as nm
import nysol.take.taxonomy as taxolib

#=アイテムクラス
# 頻出パターンマイニングで使われるアイテムを扱うクラス。
#
#===利用例
# 以下tra.csvの内容
# ---
# items
# a b c
# a c
# b
# c b d
#
# 以下taxo.csvの内容
# ---
# item,taxo
# a,X1
# b,X1
# c,X2
# d,X2
#
# 以下rubyスクリプト
# ---
# require 'rubygems'
# require 'mining'
# items=Items.new("tra.csv","items","item")
# puts items.file     # => ./1252810329_1850/dat/1
# puts items.itemFN   # => "item"
# puts items.idFN     # => "iid"
# puts items.size     # => 4
# puts items.taxonomy # =>nil
#
# taxo=Taxonomy.new("taxo.csv","item","taxo")
# items.addTaxo(taxo)
# puts items.file     # => ./1252810329_1850/dat/3
# puts items.itemFN   # => "item"
# puts items.idFN     # => "iid"
# puts items.size     # => 4
# p    items.taxonomy # => #<Taxonomy:0x1698eac @itemSize=4, @iFile="taxo.csv", @taxoFN="taxo", @itemFN="item", ...,@file="./1252810329_1850/dat/2">
#
#=====./1252810329_1850/dat/1 の内容
# item,iid
# a,1
# b,2
# c,3
# d,4
#
#=====./1252810329_1850/dat/3 の内容
# item,iid
# X1,5
# X2,6
# a,1
# b,2
# c,3
# d,4
#
class Items(object):
	"""
	:type iFile: 文字列 or リスト
	:type itemFN: 文字列
	:param iFile: transactionのCSVファイル名
	:param itemFN: 新しいアイテム項目名

	iFile上のアイテム項目(itemFN)からアイテム(itemFN)と連番("__iid")の2項目からなるファイルを生成する。
	同じアイテムが重複して登録されていれば単一化される。
	連番は、アイテム名順に昇順ソートされ、0から始まる整数が振られる。
	アイテムの件数(種類数)がセットされる。
	頻出パターンマイニングで使われるトランザクションデータを扱うクラス。
	トランザクションファイルは、トランザクションID項目とアイテム項目から構成される。
	また分類クラスや階層構造を扱うことも可能である。

	"""
	def  __init__(self,iFile,itemFN):
		self.file = None

		self.itemFN = None # アイテムの項目名(=>String)

		self.size = None # アイテムの種類数(=>Fixnum)

		self.taxonomy = None # 分類階層クラス(=>Taxonomy)

		self.temp = Mtemp()
		self.iFile   = []
		self.iPath   = []
		self.iFile.append(iFile)
		self.iPath.append(os.path.abspath(iFile))
		self.taxonomy = None
		self.file     = self.temp.file()  # 出力ファイル名
		self.itemFN   = itemFN
		self.idFN     = "__id"
		self.freqFN   = "__freq"

		nm.mcut(f=itemFN,i=iFile).mcount(k=itemFN,a=self.freqFN).mnumber(s=itemFN,a=self.idFN,S=0,o=self.file).run()

		self.size = mrecount(i=self.file) # itemの数

	def __str__(self):
		ret ="## class name: "+self.__module__+"."+self.__class__.__name__+"\n"
		ret+="id: "+"%x"%id(self)+"\n"
		ret+="iFile: "+",".join(self.iFile)+"\n"
		ret+="size: "+str(self.size)+"\n"
		return ret

	#==アイテムの追加
	# iFileのitemFN項目をアイテムとして追加する。
	#=====引数
	# iFile: アイテム項目を含むファイル名
	# itemFN: iFile上のアイテム項目名
	#=====機能
	#* itemFNとしては、スペースで区切られた複数のアイテムであってもよい。
	#* 同じアイテムが重複していれば単一化される。
	#* 追加後アイテム順にソートされる。
	#* アイテム数が更新される。
	def add(self,iFile,itemFN):

		self.iFile.append(iFile)
		self.iPath.append(os.path.abspath(iFile))

		xx=Mtemp()
		xx1=xx.file()
		xx2=xx.file()
		x0 =   nm.mcommon(k=self.itemFN,i=iFile,m=self.file)
		x0 <<= nm.mcut(f=itemFN+":"+self.itemFN)
		x0 <<= nm.muniq(k=self.itemFN)
		x0 <<= nm.mnumber(s=self.itemFN,S=self.size+1,a=self.idFN,o=xx1)
		x0.run()

		nm.mcat(i=self.file+","+xx1).msortf(f=self.itemFN,o=xx2).run()

		# 新itemファイル登録&item数更新
		shutil.move(xx2,self.file)
		self.size = mrecount(i=self.file)


	#==Taxonomyの設定
	# Taxonomy(分類階層)を設定する。
	#=====引数
	# taxo: taxonomy オブジェクト
	#=====機能
	#* Taxonomyオブジェクトをメンバとして追加する。
	#* 分類項目をアイテムとして追加する。
	def addTaxo(self,taxo):
		self.taxonomy=taxo
		self.add(self.taxonomy.file,self.taxonomy.taxoFN) # taxonomyをアイテムとして追加登録

	def repTaxo(self,taxo):

		#@taxonomy=taxo #replaceの場合はtaxonomyを登録しない
		self.file = self.temp.file() # 出力ファイル名
		nm.mcut(i=taxo.file,f=taxo.taxoFN+":"+self.itemFN).muniq(k=self.itemFN).mnumber(s=self.itemFN,a=self.idFN,S=1,o=self.file).run()
		

