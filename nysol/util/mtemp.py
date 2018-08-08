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
import glob

#= 一時ファイル名を扱うクラス (内容確認すること)
#
#  一時ファイル名の生成と、そのファイルの(自動)削除を行うクラス*。
#  一時ファイル名はfileもしくはpipeメソッドを呼び出すたびに重複なく生成される。
#  fileメソッドでは、ファイル名が生成するだけで、実ファイルは生成されない。
#  一方でpipeメソッドでは、mkfifoコマンドにより名前付きパイプファイルが生成される。
#
#  一時ファイル名の命名規則は以下の通り。
#   "#{@path}/__MTEMP_#{@pid}_#{@oid}_#{@seq}"
#
#  @pid : プロセスID ($$)
#  @oid : オブジェクトID (self.object_id)
#  @seq : オブジェクト内の通し番号 (自動採番で1から始まる)
#  @path   : 以下の優先順位で決まる。
#       1) Mtemp.newの第1引数で指定された値*
#       2) KG_TmpPath環境変数の値
#       3) TMP環境変数の値
#       4) TEMP環境変数の値
#       5) "/tmp"
#       6) "." (カレントパス)
#
#  注*) new第1引数でパス名を明示的に指定した場合、GC時に自動削除されない。
#
#=== メソッド:
# file : 一時ファイル名を返す
# path : 一時ファイル名を格納するパスを返す
# rm   : 実行時点までに生成した一時ファイルを全て削除する。
#
#=== 例1
#  基本利用例
# ------------------------------------------------------
#  require 'mtools'
#
#  tmp=MCMD::Mtemp.new
#  fName1=tmp.file
#  fName2=tmp.file
#  fName3=tmp.file("./xxa")
#  puts fName1 # -> /tmp/__MTEMP_60637_2152301760_0
#  puts fName2 # -> /tmp/__MTEMP_60637_2152301760_1
#  puts fName3 # -> ./xxa
#  File.open(fName1,"w"){|fp| fp.puts "temp1"}
#  File.open(fName2,"w"){|fp| fp.puts "temp2"}
#  File.open(fName3,"w"){|fp| fp.puts "temp3"}
#  # tmpがローカルのスコープを外れると
#  # GCが発動するタイミングで一時ファイルも自動的に削除される。
#  # ただし、fName3は一時ファイル名を直接指定しているの削除されない。
# ------------------------------------------------------
#
#===例2:
#  全ての一時ファイルが自動削除されない例
# ------------------------------------------------------
#  require 'mtools'
#
#  # コンストラクタでパスを指定すると自動削除されない。
#  # (rmメソッドにより削除することはできる。)
#  tmp=MCMD::Mtemp.new(".")
#  fName=tmp.file
#  File.open(fName,"w"){|fp| fp.puts "temp"}
#  # tmpがローカルのスコープを外れGCが発動しても
#  # 一時ファイルは削除されない。
# ------------------------------------------------------
#
#=== 例3:
#  名前付きパイプ名の生成
# ------------------------------------------------------
#  require 'mtools'
#
#  tmp=MCMD::Mtemp.new
#  pName=tmp.pipe
#  system("ls -l #{pName}") # この段階で名前付きパイプファイルが作成されている。
# ->  prw-r--r--  1 user  group  0  7 19 12:20 /tmp/__MTEMP_60903_2152299720_0
#  system("echo 'abc' > #{pName} &") # バックグラウンド実行でnamed pipeに書き込み
#  system("cat <#{pName} &")         # バックグラウンド実行でnamed pipeから読み込み
#  # tmpがローカルのスコープを外れると
#  # GCが発動するタイミングで全ての一時ファイルは自動削除される。
# ------------------------------------------------------
class Mtemp(object):

	def __init__(self,path=None) :
		self.gcRM=True
		defenv = os.environ

		if path :
			self.path=path
			self.gcRM=False

		elif "KG_TmpPath" in defenv :
			self.path=defenv["KG_TmpPath"]

		elif "TMP" in defenv :
			self.path=ENV["TMP"]

		elif "TEMP" in defenv :
			self.path=ENV["TEMP"]

		elif os.access("/tmp",os.W_OK) :
			self.path="/tmp"

		elif os.access(".",os.W_OK) :
			self.path="."

		else:
			raise("no writable temporal directory found")


		self.pid  = os.getpid()
		self.oid  = id(self)

		self.seq = 0 # オブジェクト内通し連番


	# ワークファイルを全て消す
	def delAllFiles(self,path,pid,oid):
		if  self.pid == os.getpid() and self.oid == id(self):

			for dn in glob.glob("%s/__MTEMP_%s_%s_*"%(path,self.pid,self.oid)):
				os.system("rm -rf %s"%(dn))

	def __del__(self):
		self.delAllFiles(self.path,self.pid,self.oid)		

	def mkname(self):
		return "%s/__MTEMP_%s_%s_%s"%(self.path,self.pid,self.oid,self.seq)

	#== 一時ファイル名の取得
	# 返値: 一時ファイル名(String)
	#
	# 以下のフォーマットで一時ファイル名を生成する。
	# seqはカウントアップされる。
	# フォーマット: "#{@path}/__MTEMP_#{@pid}_#{@oid}_#{@seq}"
	# nameが指定されれば(@path以外に)GCで削除しなくなる。
	def file(self,name=None):
		n=None
		if name==None:
			n= self.mkname()
			self.seq += 1
		else:
			n=name

		return n

	#== 一時ファイル名(名前付きパイプ)の取得 deprecated
	# 返値: 一時ファイル名(String)
	#
	# 以下のフォーマットで名前付きパイプの一時ファイル名を生成する。
	# @seqはカウントアップされる。
	# フォーマット: "#{@path}/__MTEMP_#{@pid}_#{@oid}_#{@seq}"
	# nameが指定されれば(@path以外に)GCで削除しなくなる。
	def pipe(self,name=None):
		n=None
		if name==None :
			n=self.mkname()
			self.seq += 1
		else :
			n=name

		os.system("mkfifo %s"%(n))
		return n

	#== 一時ファイルの出力パスの取得
	# 返値: 一時ファイルを出力パス名(String)
	def path(self):
		return self.path

 	#=== 一時ファイルの削除
 	# 以下のコマンドを実行することで一時ファイルを全て削除する。
	# system "rm -rf #{path}/__MTEMP_#{@pid}_#{@oid}_*"
	def rm(self):
		self.delAllFiles(self.path,self.pid,self.oid)

	# ワークファイル強制削除#{path}
	def forceDel(self,save=False):
		for fn in glob.glob("%s/__MTEMP_*"%(self.path)):
			if save :
				os.system("tar cvfzP %s.tar.gz  %s"%(fn,fn))

			os.system("rm -rf %s"%(fn))


#==============================================      
#以下、サンプルコード
#==============================================      
if __name__ == '__main__':
	import os
	tmp=Mtemp()
	fName1=tmp.file()
	fName2=tmp.file()
	fName3=tmp.file("./xxa")
	print(fName1)
	print(fName2)
	print(fName3)

	with open(fName1,'w') as f:
		f.write("temp1\n")
	with open(fName2,'w') as f:
		f.write("temp2\n")
	with open(fName3,'w') as f:
		f.write("temp3\n")

	tmp=Mtemp(".")
	fName=tmp.file()

	with open(fName,'w') as f:
		f.write("temp\n")

	tmp=Mtemp()
	pName=tmp.pipe()
	os.system("ls -l %s"%(pName))
	os.system("echo 'abc' > %s &"%(pName))
	os.system("cat <%s &"%(pName))

# サンプルコード
#==============================================

