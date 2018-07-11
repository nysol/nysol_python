#!/usr/bin/env ruby
# encoding: utf-8
import os
import nysol.mcmd as nm
import nysol.util.mtemp as mtemp

#=分類階層(taxonomy)クラス
# 一階層の分類階層(アイテム-分類)を扱うクラス。
#
#===利用例
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
# taxo=Taxonomy("taxo.csv","item","taxo")
# puts taxo.itemFN   # => "item"
# puts taxo.taxoFN   # => "taxo"
# puts taxo.itemSize # => 4 (アイテムは"a,b,c,d"の4種類)
# puts taxo.taxoSize # => 2 (分類は"X1,X2"の2種類)
# puts taxo.file     # => ./1252379737_1756/dat/1
#
#=====./1252379737_1756/dat/1 の内容
# item,taxo
# a,X1
# b,X1
# c,X2
# d,X2
class Taxonomy(object):


	#=== taxonomyクラスの初期化
	# 一階層の分類階層を扱う。
	# アイテム項目とその分類名項目を持つファイルから分類階層オブジェクトを生成する。
	#====引数
	# iFile: taxonomyファイル名
	# itemFN: アイテム項目名
	# taxoFN: 分類項目名
	#====機能
	#* アイテム(itemFN)と分類(taxoFN)の2項目からなるファイルが規定のパス(Taxonomy.file)に書き出される。
	#* 同じアイテムが重複して登録されていれば単一化して書き出される。
	#* アイテム順にソートされる。
	#* アイテム数と分類数を計算する。
	def __init__(self,iFile,itemFN,taxoFN):
		# アイテムの項目名(=>String)
		self.itemFN = None

		# 分類の項目名(=>String)
		self.taxoFN = None

		# アイテムの種類数(=>Fixnum)
		self.itemSize = None

		# 分類の種類数(=>Fixnum)
		self.taxoSize = None

		# taxonomyデータファイル名(=>String)
		self.file = None

		self.temp = mtemp.Mtemp()

		self.iFile  = iFile

		self.iPath  = os.path.abspath(self.iFile)
		self.itemFN  = itemFN
		self.taxoFN  = taxoFN

		# item順に並べ替えてpathに書き出す
		self.file = self.temp.file()
		para_it = self.itemFN +"," + self.taxoFN
		nm.mcut(f=para_it,i=self.iFile).muniq(k=para_it,o=self.file).run(msg="on")
	

		f = nm.mcut(f=self.itemFN,i=self.iFile)
		f <<= nm.mtrafld(f=self.itemFN,a="__fld",valOnly=True)
		f <<= nm.mtra(f="__fld",r=True)
		f <<= nm.muniq(k="__fld")
		f <<= nm.mcount(a="size")
		f <<= nm.mcut(f="size")
		xx1 = f.run()

		self.itemSize = int(xx1[0][0])

		xx2 = nm.mcut(f=self.taxoFN+":item",i=self.file).muniq(k="item").mcount(a="size").mcut(f="size").run()
		self.taxoSize = int(xx2[0][0])


