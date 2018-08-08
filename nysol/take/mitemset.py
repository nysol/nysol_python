#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import os.path
import nysol.mcmd as nm
import nysol.util.margs as margs
from nysol.take.lib.base import traDB as tdb
from nysol.take.lib import taxonomy as taxolib
from nysol.take.lib import enumLcmIs as elcmIs
from nysol.take.lib import enumLcmEp as elcmEp


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

	paramter = {	
		"i": "filename",
		"x": "fileName",
		"O": "str",
		"tid": "fld",
		"item":"fld",
		"cls" :"fld",
		"taxo":"fld",
		"type":"str",
		"s" : "float",
		"p": "float",
		"uniform" : "bool",
		"replaceTaxo": "bool",
		"S":"int",
		"sx":"float",
		"Sx":"int",
		"g":"float",
		"l":"int",
		"u":"int",
		"top":"int",
		"mcmdenv" : "bool",
		"T": "str"
	}
	paramcond = {	
		"hissu":"i"
	}
	
	def help():
		print(mitemset.helpMSG) 

	def ver():
		print(mitemset.versionInfo)

	
	def __param_check_set(self , kwd):

		for k,v in kwd.items():
			if not k in mitemset.paramter	:
				raise( Exception("KeyError: {} in {} ".format(k,self.__class__.__name__) ) )
			#型チェック入れる
			
		import datetime
		t = datetime.datetime.today()

		self.iFile   = kwd["i"]
		self.xFile   = kwd["x"]    if "x"    in kwd else None
		self.idFN    = kwd["tid"]  if "tid"  in kwd else "tid"
		self.itemFN  = kwd["item"] if "item" in kwd else "item"
		self.clsFN   = kwd["cls"]  if "cls"  in kwd else None

		self.taxoFN  = kwd["taxo"] if "taxo" in kwd else "taxo"
		self.outPath = kwd["O"]    if "O"    in kwd else "./take_{}".format(t.strftime("%Y%m%d%H%M%S"))
		
		self.eArgs={}
		self.eArgs["nomodel"] = True

		self.eArgs["type"   ] = kwd["type"]     if "type"    in kwd else "F"
		self.eArgs["minSup" ] = float(kwd["s"]) if "s"       in kwd else 0.05  # 0-1
		self.eArgs["minProb"] = float(kwd["p"]) if "p"       in kwd else 0.5   # 0.5-1
		self.eArgs["uniform"] = kwd["uniform"]  if "uniform" in kwd else False

		if "S" in kwd :
			self.eArgs["minCnt"] = int(kwd["S"])

		if "sx" in kwd :
			self.eArgs["maxSup"] = float(kwd["sx"]) # 0-1

		if "Sx" in kwd :
			self.eArgs["maxCnt"] = int(kwd["Sx"]) # 1-

		if "g" in kwd :
			self.eArgs["minGR"] = float(kwd["g"]) # 1.0

		if "l" in kwd :
			self.eArgs["minLen"] = int(kwd["l"]) # 1-

		if "u" in kwd :
			self.eArgs["maxLen"] = int(kwd["u"]) # 1-

		if "top" in kwd :
			self.eArgs["top"] = int(kwd["top"])


		self.replaceTaxo = kwd["replaceTaxo"] if "replaceTaxo" in kwd else False



	def __init__(self,**kwd):
		#パラメータチェック
		self.args = kwd
		self.__param_check_set(kwd)


	def run(self):
		# V型DBの読み込み
		db = tdb.TraDB(self.iFile,self.idFN,self.itemFN,self.clsFN)
		# taxonomyのセット
		self.taxo=None
		if self.xFile!=None :
			self.taxo = taxolib.Taxonomy(self.xFile,self.itemFN,self.taxoFN)
			if self.replaceTaxo :
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
		if not os.path.isdir(self.outPath) :
			os.makedirs(self.outPath)
		lcm.output(self.outPath)

