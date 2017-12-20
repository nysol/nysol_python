#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import nysol.mod as nm
import nysol.util.margs as margs
import nysol.take.lib.traDB as tdb
import nysol.take.lib.taxonomy as taxolib
import nysol.take.lib.enumLcmIs as elcmIs
import nysol.take.lib.enumLcmEp as elcmEp


class mitemset(object):

	helpMSG="""
----------------------------
#{$cmd} version #{$version}
----------------------------
概要) LCMにより多頻度アイテム集合を列挙する
特徴) 1) 分類階層を扱うことが可能
      2) 頻出パターン, 飽和頻出パターン, 極大頻出パターンの３種類のパターンを列挙可能
      3) クラスを指定することで、上記3パターンに関する顕在パターン(emerging patterns)を列挙可能
書式) #{$cmd} i= [x=] [O=] [tid=] [item=] [taxo=] [class=] [type=] [s=|S=] [sx=|Sx=] [l=] [u=] [p=] [top=] [-replaceTaxo] [T=] [--help]

例) #{$cmd} i=basket.csv tid=traID item=商品

  ファイル名指定
  i= : アイテム集合データベースファイル名【必須】
  x= : taxonomyファイル名【オブション】*1
  O= : 出力ディレクトリ名【オプション:default:./take_現在日付時刻】

  項目名指定
  tid=   : トランザクションID項目名(i=上の項目名)【オプション:default="tid"】
  item=  : アイテム項目名(i=,t=上の項目名)【オプション:default="item"】
  class= : クラス項目名(i=上の項目名)【オプション:default="class"】
  taxo=  : 分類項目名を指定する(x=上の項目名)【条件付き必須:x=】

  列挙パラメータ
  type= : 抽出するパターンの型【オプション:default:F, F:頻出集合, C:飽和集合, M:極大集合】
  s=    : 最小支持度(全トランザクション数に対する割合による指定)【オプション:default:0.05, 0以上1以下の実数】
  S=    : 最小支持度(件数による指定)【オプション】
  sx=   : 最大支持度(support)【オプション:default:1.0, 0以上1以下の実数】
  Sx=   : 最大支持件数【オプション】
  l=    : パターンサイズの下限(1以上20以下の整数)【オプション:default:制限なし】
  u=    : パターンサイズの上限(1以上20以下の整数)【オプション:default:制限なし】
  p=    : 最小事後確率【オプション:default:0.5】
  g=    : 最小増加率【オプション】
  top=  : 列挙するパターン数の上限【オプション:default:制限なし】*2

  その他
  -replaceTaxo : taxonomyを置換する
  T= : ワークディレクトリ(default:/tmp)
  --help : ヘルプの表示

  注釈
  *1 x=が指定されたとき、itemに対応するtaxonomyをトランザクションに追加して実行する。例えば、アイテムa,bのtaxonomyをX、c,dのtaxonomyをYとすると、あるトランザクションabdはabdXYとなる。
     ただし-replaceTaxoが指定されると、taxonomyは追加ではなく置換して実行する。前例ではトランザクションabdはXYに置換される。
  *2 top=が指定された時の動作: 例えばtop=10と指定すると、支持度が10番目高いパターンの支持度を最小支持度として頻出パターンを列挙する。よって、同じ支持度のパターンが複数個ある場合は10個以上のパターンが列挙されるかもしれない。

# より詳しい情報源 http://www.nysol.jp
# LCMの詳しい情報源 http://research.nii.ac.jp/~uno/codes-j.htm
# Copyright(c) NYSOL 2012- All Rights Reserved.
		"""

	verInfo="version=1.2"
	
	def help():
		print(mitemset.helpMSG) 

	def ver():
		print(mitemset.versionInfo)


	def __init__(self,args):
		self.args = args
		self.iFile   = args.file("i=","r")
		self.xFile   = args.file("x=","r")
	
		import datetime
		t = datetime.datetime.today()
		self.outPath = args.file("O=", "w", "./take_%s"%(t.strftime("%Y%m%d%H%M%S")))

		self.idFN   = args.field("tid=",  self.iFile, "tid"  )
		self.itemFN = args.field("item=", self.iFile, "item" )
		self.clsFN  = args.field("class=",self.iFile, None )
		self.taxoFN = args.field("taxo=", self.xFile, "taxo" )
		if self.idFN :
			self.idFN   = ','.join(self.idFN["names"])
		if self.itemFN :
			self.itemFN = ','.join(self.itemFN["names"])
		if self.clsFN :
			self.clsFN  = ','.join(self.clsFN["names"]) 
		if self.taxoFN:
			self.taxoFN = ','.join(self.taxoFN["names"]) 

		self.eArgs={}
		self.eArgs["type"   ] = args.  str("type=","F" )
		self.eArgs["minSup" ] = args.float("s="   ,0.05 ,0  ,1      ) # 最小サポート
		self.eArgs["minProb"] = args.float("p="   ,0.5  ,0.5,1      ) # 最小事後確率
		self.eArgs["uniform"] = args. bool("-uniform") # クラス事前確率を一様と考えるかどうか
		self.eArgs["nomodel"] = True

		val =  args.int("S=",None,1,None) # 最小サポート件数
		if val!=None:
			self.eArgs["minCnt"] = val

		val = args.float("sx=",None,0,1) # 最小サポート
		if val!=None:
			self.eArgs["maxSup"] = val 

		val = args.int("Sx=",None,1,None) # 最小サポート件数
		if val!=None:
			self.eArgs["maxCnt"] = val

		val = args.float("g=",None,1.0,None) # 最小GR
		if val!=None:
			self.eArgs["minGR"] = val

		val = args.int("l=",None,1,None)
		if val!=None:
			self.eArgs["minLen"] = val

		val = args.int("u=",None,1,None)
		if val!=None:
			self.eArgs["maxLen"] = args.int("u=",None,1,None)

		val = args.int("top=",None,0)
		if val!=None:
			self.eArgs["top"] = val



	def run(self):
		# V型DBの読み込み
		db = tdb.TraDB(self.iFile,self.idFN,self.itemFN,self.clsFN)
		# taxonomyのセット
		self.taxo=None
		if self.xFile!=None :
			self.taxo = taxolib.Taxonomy(self.xFile,self.itemFN,self.taxoFN)
			if self.args.bool("-replaceTaxo") :
				db.repTaxo(self.taxo) # taxonomyの置換
			else:
				db.addTaxo(self.taxo) # taxonomyの追加

		# パターン列挙
		lcm=None
		if self.clsFN :
			lcm=elcmEp.LcmEp(db)
			lcm.enumerate(self.eArgs)
		else:
			lcm=elcmIs.LcmIs(db);
			lcm.enumerate(self.eArgs)
		# 出力
		os.system("mkdir -p %s"%(self.outPath))
		lcm.output(self.outPath)


if __name__ == '__main__':
	import sys
	args=nu.Margs(sys.argv,"i=,x=,O=,tid=,item=,class=,taxo=,type=,s=,S=,sx=,Sx=,g=,p=,-uniform,l=,u=,top=,T=,-replaceTaxo")
	mitemset(args).run()

