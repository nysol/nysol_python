#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
グラフ構造データクラス
"""
__author__  = "Hiroaki Maruhashi <info@nysol.jp>"
__version__ = "1.00"
__date__    = "28 Dec 2017"

import os
import nysol.mcmd as nm
import nysol.util.mtemp as mtemp

class graph(object):

	def __init__(self,edgeFile=None,edgeList=None,title1=None,title2=None,nodeFile=None,nodeList=None,title=None):
		#メンバ変数定義
		self.eFileT		= None # Take内部で使用するエッジファイル(アイテム間をスペース区切りにしたファイル)
		self.eFile		= None # エッジファイル
		self.eFileN		= None # エッジファイル名
		self.edgeFN1	= None # エッジファイル項目名1(String)
		self.edgeFN2	= None # エッジファイル項目名2(String)
		self.nFile		= None # ノードファイル
		self.nFileN		= None # ノードファイル名
		self.nodeFN		= None # ノードファイル項目名(String)
		self.mFile		= None # ノードIDマッピングファイル
		self.__wfE		= None # エッジ用ワークファイル
		self.__wfN		= None # ノード用ワークファイル
		self.__tempW	= mtemp.Mtemp()
		self.__temp		= mtemp.Mtemp()
		#引数指定があればデータ読み込み(ファイルとリストが同時に指定された場合はファイル優先)
		#エッジ読み込み

		if edgeFile is not None:
			self.__readEdgeCSV(edgeFile=edgeFile,title1=title1,title2=title2)
		else:
			if edgeList is not None:
				self.__readEdgeList(edgeList=edgeList,title1=title1,title2=title2)
		#ノード読み込み
		if nodeFile is not None:
			self.__readNodeCSV(nodeFile=nodeFile,title=title)
		else:
			if nodeList is not None:
				self.__readNodeList(nodeList=nodeList,title=title)
		#符号化
		if self.__wfE is not None:
		  self.__convertToNumeric()

	def __readEdgeCSV(self,edgeFile=None,title1=None,title2=None):
		self.eFileN	= edgeFile
		if title1 is None or title2 is None :
			mcmd = nm.mcut(nfni=True,f="0:e1,1:e2",i=self.eFileN)
		else :
			self.edgeFN1 = title1
			self.edgeFN2 = title2
			mcmd = nm.mcut(f="%s:e1,%s:e2"%(self.edgeFN1,self.edgeFN2),i=self.eFileN)
		self.__wfE = self.__tempW.file()
		mcmd <<= nm.mnumber(q=True,a="no",o=self.__wfE)
		mcmd.run()

	def __readEdgeList(self,edgeList=None,title1=None,title2=None):
		if title1 is not None and title2 is not None :
			self.edgeFN1 = title1
			self.edgeFN2 = title2
		mcmd = nm.mcut(nfni=True,f="0:e1,1:e2",i=edgeList)
		self.__wfE = self.__tempW.file()
		mcmd <<= nm.mnumber(q=True,a="no",o=self.__wfE)
		mcmd.run()

	def __readNodeCSV(self,nodeFile=None,title=None):
		if nodeFile is not None:
			self.nFileN	= nodeFile
			if title is None :
				mcmd = nm.mcut(nfni=True,f="0:n",i=self.nFileN)
			else :
				self.nodeFN = title
				mcmd = nm.mcut(f="%s:n"%(self.nodeFN),i=self.nFileN)
			self.__wfN = self.__tempW.file()
			mcmd <<= nm.mnumber(q=True,a="no",o=self.__wfN)
			mcmd.run()
	
	def __readNodeList(self,nodeList=None,title=None):
		if nodeList is not None:
			if title is not None :
				self.nodeFN = title
			self.__wfN = self.__tempW.file()
			mcmd =   nm.m2cross(nfn=True,a="num,node",f="0-%d"%(len(nodeList)-1),i=[nodeList])
			mcmd <<= nm.mcut(nfni=True,f="1:n,0:no",o=self.__wfN)
			mcmd.run()
	
	def __convertToNumeric(self):
		wf1 = self.__tempW.file()
		wf2 = self.__tempW.file()
		wf3 = self.__tempW.file()
		self.mFile = self.__temp.file()
		nm.mcut(f="e1:node",i=self.__wfE,o=wf1).run()
		nm.mcut(f="e2:node",i=self.__wfE,o=wf2).run()
		mcmd = None
		if self.__wfN is not None:
			nm.mcut(f="n:node",i=self.__wfN,o=wf3).run()
			mcmd <<= nm.mcat(i=wf1+","+wf2+","+wf3,f="node")
		else:
			mcmd <<= nm.mcat(i=wf1+","+wf2,f="node")
		mcmd <<= nm.muniq(k="node")
		mcmd <<= nm.mnumber(q=True,a="id",o=self.mFile)
		mcmd.run()
		#エッジファイル変換・保存
		self.eFile = self.__temp.file()
		mcmd =   nm.mjoin(i=self.__wfE,m=self.mFile,f="id:id1",k="e1",K="node")
		mcmd <<= nm.mjoin(m=self.mFile,f="id:id2",k="e2",K="node")
		mcmd <<= nm.muniq(k="id1,id2")
		mcmd <<= nm.msortf(f="id1%n,id2%n")
		mcmd <<= nm.mcut(f="id1:e1,id2:e2,no:row_index",o=self.eFile)
		mcmd.run()
		#take内部用エッジファイル保存
		self.eFileT = self.__temp.file()
		mcmd =   nm.mcal(i=self.eFile,c="cat(\" \",$s{e1},$s{e2})",a="edge")
		mcmd <<= nm.mcut(nfno=True,f="edge",o=self.eFileT)
		mcmd.run()
		#ノードファイル変換・保存
		self.nFile = self.__temp.file()
		if self.__wfN is not None:
			mcmd =   nm.mjoin(i=self.__wfN,m=self.mFile,f="id",k="n",K="node")
			mcmd <<= nm.muniq(k="id")
			mcmd <<= nm.msortf(f="id%n")
			mcmd <<= nm.mcut(f="id:n,no:row_index",o=self.nFile)
			mcmd.run()
		else:
			#エッジよりノード生成
			wf4 = self.__tempW.file()
			wf5 = self.__tempW.file()
			nm.mcut(i=self.eFile,f="e1:n",o=wf4).run()
			nm.mcut(i=self.eFile,f="e2:n",o=wf5).run()
			mcmd =   nm.mcat(i="%s,%s"%(wf4,wf5))
			mcmd <<= nm.muniq(k="n")
			mcmd <<= nm.msortf(f="n%n")
			mcmd <<= nm.mnumber(q=True,a="row_index",o=self.nFile)
			mcmd.run()
			
		#ワークファイル削除
		self.__tempW.rm()

	def readCSV(self,edgeFile=None,title1=None,title2=None,nodeFile=None,title=None):
		#グラフデータ読み込み済の場合は何もしない
		if self.eFile is not None:
			return
		#エッジファイル読み込み
		if edgeFile is None:
			raise Exception("edgeFile is mandatory.")
		else:
			self.__readEdgeCSV(edgeFile=edgeFile,title1=title1,title2=title2)
		#ノードファイル読み込み
		self.__readNodeCSV(nodeFile=nodeFile,title=title)
		#符号化
		self.__convertToNumeric()

	def readList(self,edgeList=None,title1=None,title2=None,nodeList=None,title=None):
		#グラフデータ読み込み済の場合は何もしない
		if self.eFile is not None:
			return
		#エッジリスト読み込み
		if edgeList is None:
			raise Exception("edgeList is mandatory.")
		else:
			self.__readEdgeList(edgeList=edgeList,title1=title1,title2=title2)
		#ノードリスト読み込み
		self.__readNodeList(nodeList=nodeList,title=title)
		#符号化
		self.__convertToNumeric()

	def features(self):
		nodeSize = int(nm.mcount(i=self.nFile,a="cnt").mcut(f="cnt").run()[0][0])
		edgeSize = int(nm.mcount(i=self.eFile,a="cnt").mcut(f="cnt").run()[0][0])
		dens=None
		if(nodeSize>1):
			dens=float(edgeSize)/(nodeSize*(nodeSize-1)/2.0)
		return nodeSize,edgeSize,dens

	def isSame(self,targetGraph):
		if os.path.getsize(self.eFile)!= os.path.getsize(targetGraph.eFile):
			return False
		else:
			xxt = mtemp.Mtemp()
			xx = xxt.file()
			os.system("diff -q %s %s > %s"%(self.eFile,targetGraph.eFile,xx))
			if(os.path.getsize(xx) != 0):
				xxt.rm()
				return False
			else:
				xxt.rm()
				return True
	
	def node_ids(self):
		if self.nFile is not None:
			return nm.mcut(i=self.nFile,f="n")
	
	def nodes(self):
		if self.nFile is not None:
			return (nm.mjoin(i=self.nFile,m=self.mFile,k="n",K="id",f="node")
								.msortf(f="row_index%n")
								.mcut(f="node:n"))
	
	def edge_ids(self):
		#return nm.mcat(i=self.eFile).run()
		if self.eFile is not None:
			return nm.mcut(i=self.eFile,f="e1,e2")
	
	def edges(self):
		if self.eFile is not None:
			return (nm.mjoin(i=self.eFile,m=self.mFile,k="e1",K="id",f="node:n1")
								.mjoin(m=self.mFile,k="e2",K="id",f="node:n2")
								.msortf(f="row_index%n")
								.mcut(f="n1:e1,n2:e2"))
	
	def printMap(self):
		if self.mFile is not None:
			for r in nm.mcut(i=self.mFile,f="id,node"):
				print(r)

	def printEdges(self):
		if self.eFile is not None:
			for r in nm.mcut(f="e1,e2",i=self.eFile):
				print(r)

	def printEdgesTake(self):
		if self.eFileT is not None:
			for r in nm.mcut(i=self.eFileT,nfni=True,f="0:edge"):
				print(r)

	def printNodes(self):
		if self.nFile is not None:
			for r in nm.mcut(f="n",i=self.nFile):
				print(r)

