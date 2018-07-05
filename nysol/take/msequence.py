#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import nysol.mod as nm
from nysol.take.lib.base import seqDB as sdb
from nysol.take.lib import taxonomy as taxolib
from nysol.take.lib import enumLcmSeq as elcmSeq
from nysol.take.lib import enumLcmEsp as elcmEsp
from nysol.take import extcore as extTake


class msequence(object):
	def help(self):
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
	def ver(self):
		print("version #{$version}")

# lcm_seqコマンド実行可能確認
#exit(1) unless(MCMD::chkCmdExe(TAKE::LcmSeq::CMD      , "executable"))
#exit(1) unless(MCMD::chkCmdExe(TAKE::LcmSeq::CMD_ZERO , "executable"))
#exit(1) unless(MCMD::chkCmdExe(TAKE::LcmSeq::CMD_TRANS, "-v", "lcm_trans 1.0"))


	def __init__(self,args):
		self.args = args

		# mcmdのメッセージは警告とエラーのみ
		#ENV["KG_VerboseLevel"]="2" unless args.bool("-mcmdenv")
		#ワークファイルパス
		#if args.str("T=")!=nil then
		#	ENV["KG_TmpPath"] = args.str("T=").sub(/\/$/,"")
		#end

		self.iFile   = args.file("i=","r")
		self.xFile   = args.file("x=","r")

		import datetime
		t = datetime.datetime.today()
		self.outPath = args.file("O=", "w", "./take_%s"%(t.strftime("%Y%m%d%H%M%S")))

		self.idFN   = args.field("tid=",  self.iFile, "tid"  )
		self.timeFN = args.field("time=", self.iFile, "time" )
		self.itemFN = args.field("item=", self.iFile, "item" )
		self.clsFN  = args.field("class=",self.iFile, None )
		self.taxoFN = args.field("taxo=", self.xFile, "taxo" )

		if self.idFN :
			self.idFN   = ','.join(self.idFN["names"])

		if self.timeFN:
			self.timeFN = ','.join(self.timeFN["names"])

		if self.itemFN :
			self.itemFN = ','.join(self.itemFN["names"])

		if self.clsFN :
			self.clsFN  = ','.join(self.clsFN["names"]) 

		if self.taxoFN:
			self.taxoFN = ','.join(self.taxoFN["names"]) 

		self.eArgs={}
		self.eArgs["minSup" ] = args.float("s="   ,0.05 ,0,1    ) # 最小サポート
		self.eArgs["minProb"] = args.float("p="   ,0.5  ,0.5,1  ) # 最小事後確率
		self.eArgs["uniform"] = args. bool("-uniform") # クラス事前確率を一様と考えるかどうか
		self.eArgs["padding"] = args. bool("-padding") # 0item ommit

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

		val = args.int("gap=",None,0,None) # gap長上限限
		if val!=None:
			self.eArgs["gap"] = val

		val = args.int("win=",None,0,None) # win size上限限
		if val!=None:
			self.eArgs["win"] = val

		val = args.int("top=",None,0)
		if val!=None:
			self.eArgs["top"] = val

		if "minLen" in self.eArgs and "maxLen" in self.eArgs:
			if self.eArgs["minLen"] > self.eArgs["maxLen"]:
				raise Exception("u= must be greater than or equal to l=")

		if "gap" in self.eArgs and "win" in self.eArgs:
			if self.eArgs["gap"] > self.eArgs["win"] :
				raise Exception("win= must be greater than or equal to gap=")

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


		#MCMD::msgLog("The final results are in the directory `#{outPath}'")

		# 終了メッセージ
		#MCMD::endLog(args.cmdline)

if __name__ == '__main__':
	import sys
	import nysol.util.margs as margs
	args=margs.Margs(sys.argv,"i=,c=,x=,O=,tid=,time=,item=,class=,taxo=,s=,S=,sx=,Sx=,g=,p=,-uniform,l=,u=,top=,gap=,win=,-padding,T=,-mcmdenv","i=")
	msequence(args).run()

