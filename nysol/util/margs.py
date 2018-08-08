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
#= コマンドライン引数の型チェックと設定を扱うクラス
#
# コマンドライン引数は"key=value"、"-key"のいずれかの形式を想定している。
# Unix系OSの引数の標準形式とは異なることに注意する。
#
# チェック内容:
#   1. 指定可能な引数を指定し、それ以外の引数がコマンドラインで指定されればエラー終了(エラーをraise)する。
#   2. 必須の引数を指定し、コマンドラインで指定されていなければエラー終了させる。
#   3. 引数の型を設定し、型に応じた値チェック及び変換をおこなう。
#
# 扱える型:
#   rubyの原始型として:
#      1.str: rubyのString型に変換: デフォルト値の指定 
#      2.int: rubyのFixnum型に変換: デフォルト値の指定、有効な範囲チェック
#      3.float: rubyのFloat型に変換: デフォルト値の指定、有効な範囲チェック
#      4.bool: rubyのtrue/falseに変換: -key タイプのオプションが指定されたかどうかの真偽判定
#   特殊な型として:
#      5.file: rubyのString型に変換: ファイルのreadable or writableチェック
#
#=== 例1
# コマンドライン
# ------------------------------------------------------
# $ ruby test.rb v=0.2 -w
# ------------------------------------------------------
#
# test.rbの内容
# ------------------------------------------------------
# require 'nysol/mcmd'
# include MCMD
#
# args=Margs.new(ARGV)
# val  = args.float("v=") # -> 0.2
# flag = args.bool("-w") # -> true
# ------------------------------------------------------
#
#=== 例2 引数存在チェックや型チェックの例
# コマンドライン
# ------------------------------------------------------
# $ ruby test.rb i=dat.csv v=value -abc
	# ------------------------------------------------------
#
# test.rbの内容
# ------------------------------------------------------
# require 'nysol/mcmd'
# include MCMD
#
# # "i=,o=,w=,-flag,-x"以外の引数が指定されればエラー終了する。
# # "i=,w="引数を指定しなければエラー終了する。
# args=Margs.new(ARGV, "i=,o=,w=,-flag,-x", "i=,w=")
# iFileName = args.file("i=") # -> "dat.csv"
# oFileName = args.str("o=","result.csv") # -> "result.csv"
# weight    = args.float("w=",0.1,0.0,1.0) # -> 0.1
# flag      = args.bool("-abc") # -> true
# wFlag     = args.bool("-w") # -> false
# ------------------------------------------------------
import os
import re
class Margs(object): #みなおすこと

	#== コンストラクタ
	# argv: rubyのARGV変数
	#
	# allKeyWords: key=もしくは-keyによる引数キーワードリスト(String Array)
	#   ここで指定した以外の引数がARGVに指定されていないことをチェックし、指定されていればエラー終了する。
	#   keyListを省略した場合はこのチェックをしない。
	#
	# mandatoryKeyWords: key=による引数キーワードリスト(String Array)
	#   ここで指定した引数がコマンドラインで指定されていなければエラー終了する。
	#   mandatoryKeyWordsを省略した場合はこのチェックをしない。
	def __init__(self,argv, allKeyWords=None, mandatoryKeyWords=None, help_func=None,ver_func=None):
		self.argv=argv
		self.allKeyWords=allKeyWords
		self.mandatoryKeyWords=mandatoryKeyWords
		self.keyValue = {}
		self.cmdName = argv[0]

		# コマンドラインで指定された引数を一旦全てHashに格納する。
		for arg in argv[1:]:
			if arg.startswith('-') :
				self.keyValue[arg]=True
				if arg=="--help":
					if help_func :
						help_func()
						exit()
				elif arg=="--version":
					if ver_func:
						ver_func()
						exit()
			else:
				kv =arg.split("=", 2)
				if len(kv) < 1 :
					raise Exception("invalid argument: '%s'"%(arg))

				self.keyValue[kv[0]+"="]=kv[1]

		# allKeyWordsのオプションタイプのキーワードを登録する
		if self.allKeyWords!=None :
			kwlist = self.allKeyWords.split(",")
			for kw in kwlist:
				if kw.startswith('-') and kw not in self.keyValue :
					self.keyValue[kw]=False

			# 指定のキーワード以外のキーワードが指定されていないかチェック
			for kw in self.keyValue.keys():
				if kw not in kwlist:
					raise Exception("I don't know such a argument: '%s'"%(kw))

		# 必須引数のチェック #おかしいような気がする
		if self.mandatoryKeyWords !=None :
			for kw in self.mandatoryKeyWords.split(','):
				if kw not in self.keyValue and not kw.startswith('-'):
					raise Exception("argument '%s' is mandatory"%(kw))


	#== String型引数の値のチェックと取得
	# 返値: 引数で指定された値(String)
	#
	# key: "key="形式の引数キーワード(String)
	#   ここで指定した引数の値をStringとして返す。
	#   コマンドラインで指定されていなければdefaultの値を返す。
	#
	# default: コマンドラインで指定されなかったときのデフォルト値(String)
	def str(self,key,default=None,token1=None,token2=None):
		if key in self.keyValue :
			val = self.keyValue[key]
		else :
			val=default 
			return val

		if val!=None :
			if token1!= None :
				val1=val.split(token1)
				if token2!=None :
					val2=[]
					for v in val1 :
						val2.apend(v.split(token2))
					val = val2
				else:
					val = val1

		return val


	#== Fixnum型引数の値のチェックと取得
	# 返値: 引数で指定された値(Fixnum)
	#
	# key: "key="形式の引数キーワード
	#   ここで指定した引数の値をFloatとして返す。
	#   コマンドラインで指定されていなければdefaultの値を返す。
	#
	# default: コマンドラインで指定されなかったときのデフォルト値
	#
	# from: 値の下限値。指定した値が下限値を下回ればエラー終了する。
	#
	# to: 値の上限値。指定した値が上限値を上回ればエラー終了する。
	def int(self,key, default=None, fr=None, to=None):

		if key in self.keyValue :
			val = self.keyValue[key]
		else :
			val=default 

		if val != None :
			val=int(val)
			if ( fr != None and val<fr ) or ( to != None and val>to ):
				raise Exception("range error: '%s=%s': must be in [%s..%s]"%(key,val,fr,to))

		return val

	#== Float型引数の値のチェックと取得
	# 返値: 引数で指定された値(Float)
	#
	# key: "key="形式の引数キーワード
	#   ここで指定した引数の値をFloatとして返す。
	#   コマンドラインで指定されていなければdefaultの値を返す。
	#
	# default: コマンドラインで指定されなかったときのデフォルト値
	#
	# from: 値の下限値。指定した値が下限値を下回ればエラー終了する。
	#
	# to: 値の上限値。指定した値が上限値を上回ればエラー終了する。
	def float(self,key, default=None, fr=None, to=None):
		if key in self.keyValue :
			val = self.keyValue[key]
		else :
			val=default 

		if val!= None :
			val=float(val)
			if ( fr != None and val < fr ) or (to != None and val>to) :
				raise Exception("range error: '%s=%s': must be in [%s..%s]"%(key,val,fr,to))

		return val

	#== Bool型引数の値のチェックと取得
	# 返値: 引数で指定されたかどうか(true/false)
	#
	# key: "-key"形式の引数キーワード
	#   ここで指定した引数がコマンドラインで指定されていればtrueを、指定されていなければfalseを返す。
	def bool(self,key):
		if key in self.keyValue :
			return self.keyValue[key]
		else :
			return None

	#== ファイル型引数の値のチェックと取得
	# 返値: 引数で指定されたファイル名(String)
	#
	# key: "key="形式の引数キーワード
	#   ここで指定した引数の値をファイル名と想定し、そのファイルがreadable(writable)かどうかをチェックする。
	#   readable(writable)であればそのファイル名を返し、readable(writable)でなければエラー終了する。
	#   readable(writable)チェックをしないのであればMargs::strを使えばよい。
	#
	# mode: "r"もしくは"w"を指定し、rならばreadableチェックを、wならwritebleチェックを行う。
	def file(self,key,mode="r",default=None):

		if key in self.keyValue :
			val = self.keyValue[key]
		else :
			val=default 

		# valがnilの場合(ex. 値なし指定"i=")はノーチェックで通す?
		if val!=None :
			if mode=="r" :
				if not os.access(val, os.R_OK) :
					raise Exception("file open error: '%s' is not readable"%(val))

			elif mode=="w" :
				if os.path.dirname(val) == "":
					if not os.access(".", os.W_OK) :
						raise Exception("file open error: '%s' is not writable"%(val))				
				else:
					if not os.access(os.path.dirname(val), os.W_OK) :
						raise Exception("file open error: '%s' is not writable"%(val))

		return val


	#== Field型引数の値のチェックと取得
	# 返値: 各種配列のHash
	# 	key=a1:b1%c1,a2:b2%c2,...
	#   names: [a1,a2,...]
	#   newNames: [b1,b2,...]
	#   flags:  [c1,c2,...]
	#   fld2csv: a1,a2,...のCSVファイルにおける項目番号(0から始まる)
	#   csv2fld: CSVファイルの項目番号に対するa1,a2,...の番号(0から始まる)
	# PIPEつかえなくなる
	def field(self,key,iFile,default=None,min=None,max=None) :
		if not iFile :
			return None

		if key in self.keyValue :
			val = self.keyValue[key]
		else :
			val=default 

		names1=[]
		names2=[]
		flags=[]
		fld2csv=[]
		csv2fld=[]

		# names1,names2,flagsの設定
		if val!=None :
			val1=val.split(",")
			for v in val1 :
				val2=v.split("%")
				if len(val2) == 1:
					val2.append(None) 
				val3=val2[0].split(":")
				if len(val3) == 1:
					val3.append(None) 
				names1.append(val3[0])
				names2.append(val3[1])
				flags.append(val2[1])

			if min != None and len(names1)<min :
				raise Exception("%s takes at least %s field name(s)"%(key,min) )

			if max != None and len(names1)>max :
				raise Exception("%s takes at most %s field name(s)"%(key,max) )

			#dmy
			iNames=None
			"""
			iNames=MCMD::Mcsvin.new("i=#{iFile}").names
			# fld2csvの設定
			(0...names1.size).each{|i|
				pos=iNames.index(names1[i])
				if pos==nil then
					raise "field name not found: `#{names1[i]}'"
				end
				fld2csv << pos
			}

			# csv2fldの設定
			(0...iNames.size).each{|i|
				pos=fld2csv.index(i)
				if pos!=nil
					csv2fld << pos
				else
					csv2fld << nil
				end
			}
			"""

			ret={}
			ret["names"]=names1
			ret["newNames"]=names2
			ret["flags"]=flags
			ret["fld2csv"]=fld2csv
			ret["csv2fld"]=csv2fld
			ret["csvNames"]=iNames
			return ret
		else:
			return None


	# key-valuを配列で返す
	def getKeyValue(self,prefix=""):
		ret=[]
		for k,v in self.keyValue.items():
			ret.append([prefix+k,str(v)])
		return ret

	def kvmap(self):
		ret={}
		for k,v in self.keyValue.items():
			newk = re.sub(r'^-',"" ,re.sub(r'=$',"",k))
			ret[newk] =v
		return ret

	# コマンドラインをkey-valuを配列で返す
	def cmdline(self):
		return "%s %s"%(self.cmdName,' '.join(self.argv))

#==============================================
#以下、サンプルコード(require時は実行されない)
#==============================================
if __name__ == '__main__':

	# $ ruby ./margs.rb i=dat.csv v=value -abc
	import sys
	args=Margs( sys.argv, "i=,o=,w=,v=,-flag,-x,-abc", "i=,w=")


	#	args=Margs(args, "f=")
	#	fld=args.field("f=","xxa")

#	print(fld["names"])
#	print(fld["newNames"])
#	print(fld["flags"])
#	print(fld["fld2csv"])
#	print(fld["csv2fld"])

	#p args.str("f=",",",":")
	iFileName = args.file("i=") # -> "dat.csv"
	oFileName = args.str("o=","result.csv") # -> "result.csv"
	weight    = args.float("w=",0.1,0.0,1.0) # -> 0.1
	flag      = args.bool("-abc") # -> true
	wFlag     = args.bool("-w") # -> false

