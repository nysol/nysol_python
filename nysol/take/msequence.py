#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import nysol.mcmd as nm
from nysol.take.lib.base import seqDB as sdb
from nysol.take.lib import taxonomy as taxolib
from nysol.take.lib import enumLcmSeq as elcmSeq
from nysol.take.lib import enumLcmEsp as elcmEsp
from nysol.take import extcore as extTake


class msequence(object):
	def helpMSG(self):
		msg="""
------------------------
#{$cmd} version #{$version}
------------------------
概要) LCM_seqにより多頻度系列パターンを列挙する
特徴) 1) 
書式) #{$cmd} i= [x=] [O=] [tid=] [item=] [time=] [taxo=] [class=] [s=] [S=] [sx=] [Sx=] [l=] [u=] [top=] [p=] [g=] [w=] [Z=] [T=] [--help]

例1) #{$cmd} i=weblog.csv c=customer.csv tid=traID item=page time=アクセス時刻 class=購買
例2) #{$cmd} i=weblog.csv tid=traID item=page time=アクセス時刻

  ファイル名指定
  i= : アイテム集合データベースファイル名【必須】
  c= : クラスファイル名【オブション】
  x= : taxonomyファイル名【オブション】*1
  O= : 出力ディレクトリ名【オプション:default:./take_現在日付時刻】

  項目名指定
  tid=   : トランザクションID項目名(i=,c=上の項目名)【オプション:default="tid"】
  time=  : 時間項目名(i=上の項目名)【オプション:default="time"】
  item=  : アイテム項目名(i=,t=上の項目名)【オプション:default="item"】
  class= : クラス項目名(c=上の項目名)【オプション:default="class"】
  taxo=  : 分類項目名を指定する【条件付き必須:x=】

  列挙パラメータ
  s=   : 最小支持度(support)【オプション:default:0.05, 0以上1以下の実数】
  S=   : 最小支持件数【オプション】
  sx=  : 最大支持度(support)【オプション:default:1.0, 0以上1以下の実数】
  Sx=  : 最大支持件数【オプション】
  l=   : パターンサイズの下限(1以上20以下の整数)【オプション:default:制限なし】
  u=   : パターンサイズの上限(1以上20以下の整数)【オプション:default:制限なし】
  p=   : 最小事後確率【オプション:default:0.5】
  g=   : 最小増加率【オプション】
  gap= : パターンのギャップ長の上限(0以上の整数)【オプション:0で制限無し,default:0】
  win= : パターンのウィンドウサイズの上限(0以上の整数)【オプション:0で制限無し,default:0】
  -padding : 時刻を整数とみなし、連続でない時刻に特殊なアイテムがあることを想定する。
           : gap=やwin=の指定に影響する。
  top=  : 列挙するパターン数の上限【オプション:default:制限なし】*2

  その他
  T= : ワークディレクトリ(default:/tmp)
  --help : ヘルプの表示

  注釈
  *1 x=が指定されたとき、item項目の値を対応するtaxonomyに変換して実行する。例えば、アイテムa,bのtaxonomyをX、c,dのtaxonomyをYとすると、
     シーケンス"aeadd"は"XeXYY"に変換される。
  *2 top=が指定された時の動作: 例えばtop=10と指定すると、支持度が10番目高いパターンの支持度を最小支持度として頻出パターンを列挙する。よって、同じ支持度のパターンが複数個ある場合は10個以上のパターンが列挙されるかもしれない。

# より詳しい情報源 http://www.nysol.jp
# LCM_seqの詳しい情報源 http://research.nii.ac.jp/~uno/codes-j.htm
# Copyright(c) NYSOL 2012- All Rights Reserved.
		"""

	verInfo="version=1.2"


	paramter = {	
		"i": "filename",
		"c": "fileName", # なにもしてない
		"x": "fileName",
		"O": "str",
		"tid": "fld",
		"item":"fld",
		"time":"fld",
		"cls" :"fld",
		"taxo":"fld",
		"s" : "float",
		"S":"int",
		"sx":"float",
		"Sx":"int",
		"g":"float",
		"p": "float",
		"uniform" : "bool",
		"l":"int",
		"u":"int",
		"gap":"int",
		"win":"int",
		"top":"int",
		"padding" : "bool",
		"mcmdenv" : "bool",
		"T": "str"
	}
	paramcond = {	
		"hissu":"i"
	}

	def help():
		print(msequence.helpMSG) 

	def ver():
		print(msequence.versionInfo)


	def __param_check_set(self , kwd):

		for k,v in kwd.items():
			if not k in msequence.paramter	:
				raise( Exception("KeyError: {} in {} ".format(k,self.__class__.__name__) ) )
			#型チェック入れる

		import datetime
		t = datetime.datetime.today()
		self.iFile   = kwd["i"]
		self.xFile   = kwd["x"]    if "x"    in kwd else None
		self.idFN    = kwd["tid"]  if "tid"  in kwd else "tid"
		self.timeFN  = kwd["time"] if "time" in kwd else "time"
		self.itemFN  = kwd["item"] if "item" in kwd else "item"
		self.clsFN   = kwd["cls"]  if "cls"  in kwd else None

		self.taxoFN  = kwd["taxo"] if "taxo" in kwd else "taxo"
		self.outPath = kwd["O"]    if "O"    in kwd else "./take_{}".format(t.strftime("%Y%m%d%H%M%S"))

		self.eArgs={}
		self.eArgs["minSup" ] = float(kwd["s"]) if "s" in kwd else 0.05  # 0-1
		self.eArgs["minProb"] = float(kwd["p"]) if "p" in kwd else 0.5   # 0.5-1
		self.eArgs["uniform"] = kwd["uniform"] if "uniform" in kwd else False
		self.eArgs["padding"] = kwd["padding"] if "padding" in kwd else False

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

		if "gap" in kwd :
			self.eArgs["gap"] = int(kwd["gap"]) # 0-

		if "win" in kwd :
			self.eArgs["win"] = int(kwd["win"]) # 0-

		if "top" in kwd :
			self.eArgs["top"] = int(kwd["top"])

		if "minLen" in self.eArgs and "maxLen" in self.eArgs:
			if self.eArgs["minLen"] > self.eArgs["maxLen"]:
				raise Exception("u= must be greater than or equal to l=")

		if "gap" in self.eArgs and "win" in self.eArgs:
			if self.eArgs["gap"] > self.eArgs["win"] :
				raise Exception("win= must be greater than or equal to gap=")



	def __init__(self,**kwd):
		#パラメータチェック
		self.__param_check_set(kwd)


	def run(self):

		# seq型DBの読み込み
		db=sdb.SeqDB(self.iFile,self.idFN,self.timeFN,self.itemFN,self.eArgs["padding"],self.clsFN)
		#db.show

		# taxonomyのセット
		taxo=None
		if self.xFile!=None :
			self.taxo = taxolib.Taxonomy(self.xFile,self.itemFN,self.taxoFN)
			db.repTaxo(taxo) # seqはtaxonomyの置換のみ

		# パターン列挙
		lcm=None
		if self.clsFN:
			lcm=elcmEsp.LcmEsp(db);
			lcm.enumerate(self.eArgs)
		else:
			lcm=elcmSeq.LcmSeq(db);
			lcm.enumerate(self.eArgs)

		# 出力
		if not os.path.isdir(self.outPath) :
			os.makedirs(self.outPath)


		lcm.output(self.outPath)


