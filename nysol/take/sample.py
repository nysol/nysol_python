#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import os

import shutil
from nysol.util.mtemp import Mtemp
from nysol.util.mrecount import mrecount
import nysol.mcmd as nm

try:
	from igraph import *
except ImportError:
	sys.stderr.write("## warning: cannot import `igraph', so `Data.plot' method cannot be used for graph data")

try:
	import matplotlib.pyplot as plt
	import numpy as np
except ImportError:
	sys.stderr.write("## warning: cannot import `matplotlib' or `numpy', so `Data.plot' method cannot be used for transaction data ")

class Data(object):
	"""
	トランザクションデータ、およびグラフデータの様々なサンプルを生成する。

	**トランザクションデータ利用例:**

	.. doctest::

		>>> from take.sample import Data
		
		>>> dat=Data(name="tra00") # "tra00"という名前のトランザクションデータオブジェクトの生成
		>>> tadj=dat.getAdjMatrix() # 隣接行列形式(トランザクション毎のアイテムリスト)
		>>> print(tadj)
		[[2, 4], [3, 4, 5], [0, 1, 3, 5], [1, 3, 5], [0, 1, 3, 4], [0, 1, 3, 4, 5]]
		>>> tadj_a=dat.getAdjMatrix(alphabet=True) # ノードをアルファベットにした隣接行列形式(トランザクションIDをキーにした辞書型)
		>>> print(tadj_a)
		{'0': ['c', 'e'], '1': ['d', 'e', 'f'], '2': ['a', 'b', 'd', 'f'], '3': ['b', 'd', 'f'], '4': ['a', 'b', 'd', 'e'], '5': ['a', 'b', 'd', 'e', 'f']}
		>>> item=dat.getNodes() # item一覧を取得
		>>> print(item)
		[0, 1, 2, 3, 4, 5]
		>>> tra=dat.getEdges() # トランザクションIDとそれに含まれるアイテムの一覧を取得(以下の例では、トランザクションID=0に2と4のアイテムが含まれるということ)。
		>>> print(tra)
		[(0, 2), (0, 4), (1, 3), (1, 4), (1, 5), (2, 0), (2, 1), (2, 3), (2, 5), (3, 1), (3, 3), (3, 5), (4, 0), (4, 1), (4, 3), (4, 4), (5, 0), (5, 1), (5, 3), (5, 4), (5, 5)]
		>>> item_a=dat.getNodes(alphabet=True) # 上記と同様のことをアイテムをアルファベットにして出力(0から順に"a","b",...)
		>>> print(item_a)
		['a', 'b', 'c', 'd', 'e', 'f']
		>>> tra_a=dat.getEdges(alphabet=True) # トランザクションIDは整数の文字列、アイテムはアルファベット
		>>> print(tra_a)
		[('0', 'c'), ('0', 'e'), ('1', 'd'), ('1', 'e'), ('1', 'f'), ('2', 'a'), ('2', 'b'), ('2', 'd'), ('2', 'f'), ('3', 'b'), ('3', 'd'), ('3', 'f'), ('4', 'a'), ('4', 'b'), ('4', 'd'), ('4', 'e'), ('5', 'a'), ('5', 'b'), ('5', 'd'), ('5', 'e'), ('5', 'f')]
		>>> dat.writeAdjMatrix(oFile="smp_adj") # 隣接行列形式でファイル"smp_adj"に出力する
		>>> Data.head("smp_adj",top=0) # ファイルの上位top件を表示(top=0で全行表示)
		2 4
		3 4 5
		0 1 3 5
		1 3 5
		0 1 3 4
		0 1 3 4 5
		<BLANKLINE>
		>>> dat.writeAdjMatrix(oFile="smp_adja",alphabet=True)
		>>> Data.head("smp_adja",top=0)
		tid,item
		0,c e
		1,d e f
		2,a b d f
		3,b d f
		4,a b d e
		5,a b d e f
		<BLANKLINE>
		>>> dat.writeNodes(oFile="smp_items")
		>>> Data.head("items",top=0)
		0
		1
		2
		3
		4
		5
		<BLANKLINE>
		>>> dat.writeEdges(oFile="smp_tra")
		>>> Data.head("tra",top=0)
		0 2
		0 4
		1 3
		1 4
		1 5
		2 0
		2 1
		2 3
		2 5
		3 1
		3 3
		3 5
		4 0
		4 1
		4 3
		4 4
		5 0
		5 1
		5 3
		5 4
		5 5
		<BLANKLINE>
		>>> dat.writeNodes(oFile="smp_items_a",alphabet=True)
		>>> Data.head("items_a",top=0)
		node
		a
		b
		c
		d
		e
		f
		<BLANKLINE>
		>>> dat.writeEdges(oFile="smp_tra_a",alphabet=True)
		>>> Data.head("tra_a",top=0)
		tid,item
		0,c
		0,e
		1,d
		1,e
		1,f
		2,a
		2,b
		2,d
		2,f
		3,b
		3,d
		3,f
		4,a
		4,b
		4,d
		4,e
		5,a
		5,b
		5,d
		5,e
		5,f
		<BLANKLINE>
		>>> dat.plot(oFile="smp_tra00.png")

	.. figure:: ../../docs/samples/plot/tra00.png
		:scale: 100 %
		:alt: tra00.png

		tra00.png

		**グラフデータ利用例:**

	.. doctest::

		>>> dat=Data(name="graph00") # "tra00"という名前のトランザクションデータオブジェクトの生成
		>>> adj=dat.getAdjMatrix() # 隣接行列
		>>> print(adj)
		[[1, 4, 2], [2, 4, 6], [3, 5], [4], [5], [], []]
		>>> adj_a=dat.getAdjMatrix(alphabet=True) # ノードをアルファベットにした隣接行列
		>>> print(adj_a)
		{'a': ['b', 'e', 'c'], 'b': ['c', 'e', 'g'], 'c': ['d', 'f'], 'd': ['e'], 'e': ['f'], 'f': [], 'g': []}
		>>> nodes=dat.getNodes() # ノード一覧を取得
		>>> print(nodes)
		[0, 1, 2, 3, 4, 5, 6]
		>>> edges=dat.getEdges() # エッジ一覧(ノードペア)を取得
		>>> print(edges)
		[(0, 1), (0, 4), (0, 2), (1, 2), (1, 4), (1, 6), (2, 3), (2, 5), (3, 4), (4, 5)]
		>>> nodes_a=dat.getNodes(alphabet=True) # 上記と同様のことをアイテムをアルファベットにして出力(0から順に"a","b",...)
		>>> print(nodes_a)
		['a', 'b', 'c', 'd', 'e', 'f', 'g']
		>>> edges_a=dat.getEdges(alphabet=True)
		>>> print(edges_a)
		[('a', 'b'), ('a', 'e'), ('a', 'c'), ('b', 'c'), ('b', 'e'), ('b', 'g'), ('c', 'd'), ('c', 'f'), ('d', 'e'), ('e', 'f')]
		>>> dat.writeAdjMatrix(oFile="smp_adj") # 隣接行列形式でファイル"adj"に出力する
		>>> Data.head("adj",top=0) # ファイルの上位top件を表示(top=0で全行表示)
		1 4 2
		2 4 6
		3 5
		4
		5
		<BLANKLINE>
		<BLANKLINE>
		<BLANKLINE>
		>>> dat.writeAdjMatrix(oFile="smp_adj_a",alphabet=True)
		>>> Data.head("adj_a",top=0)
		node,adjacency
		a,b e c
		b,c e g
		c,d f
		d,e
		e,f
		f,
		g,
		<BLANKLINE>
		>>> dat.writeNodes(oFile="smp_nodes")
		>>> Data.head("nodes",top=0)
		0
		1
		2
		3
		4
		5
		6
		<BLANKLINE>
		>>> dat.writeNodes(oFile="smp_nodes_a",alphabet=True)
		>>> Data.head("nodes_a",top=0)
		node
		a
		b
		c
		d
		e
		f
		g
		<BLANKLINE>
		>>> dat.writeEdges(oFile="smp_edges")
		>>> Data.head("edges",top=0)
		0 1
		0 4
		0 2
		1 2
		1 4
		1 6
		2 3
		2 5
		3 4
		4 5
		<BLANKLINE>
		>>> dat.writeEdges(oFile="smp_edges_a",alphabet=True)
		>>> Data.head("edges_a",top=0)
		node1,node2
		a,b
		a,e
		a,c
		b,c
		b,e
		b,g
		c,d
		c,f
		d,e
		e,f
		<BLANKLINE>
		>>> dat.plot(oFile="smp_graph00.png")


	.. figure:: ../../docs/samples/plot/graph00.png
		:scale: 100 %
		:alt: graph00.png

		"graph00.png"

	"""

	def  __init__(self,name):
		self.name=name
		eval("self._{}()".format(self.name))
		self._parseData()

	def _parseData(self):
		if self.type=="tra":
			self.nodes=[]
			self.edges=[]
			for t in range(len(self.data)):
				for item in self.data[t]:
					self.nodes.append(int(item))
					self.edges.append((t,int(item)))
			self.nodes=list(set(self.nodes))
		else:
			self.nodes=[]
			self.edges=[]
			for n in range(len(self.data)):
				self.nodes.append(n)
				for adj in self.data[n]:
					self.edges.append((n,int(adj)))

	def _num2alpha(val):
		s=""
		while(True):
			div=int(val/26)
			sub=val%26
			s+=chr(97+sub)
			if(div<=0):
				break
			val=div-1
		return s[::-1] # reverse order

	def getAdjMatrix(self,alphabet=False):
		if alphabet:
			adjMatrix={}
			for n in range(len(self.data)):
				if self.type=="tra":
					ns=str(n)
				else:
					ns=Data._num2alpha(n)
				adjMatrix[ns]=[]
				for adj in self.data[n]:
					adjMatrix[ns].append(Data._num2alpha(adj))
			return adjMatrix

		else:
			return self.data

	def getNodes(self,alphabet=False):
		if alphabet:
			n=[]
			for node in self.nodes:
				n.append(Data._num2alpha(node))
			return n
		return self.nodes

	def getEdges(self,alphabet=False):
		if alphabet:
			e=[]
			for edge in self.edges:
				if self.type=="tra":
					e.append((str(edge[0]),Data._num2alpha(edge[1])))
				else:
					e.append((Data._num2alpha(edge[0]),Data._num2alpha(edge[1])))
			return e
		return self.edges

	def writeAdjMatrix(self,oFile,alphabet=False):
		with open(oFile,"w") as fpw:
			if alphabet:
				if self.type=="tra":
					fpw.write("tid,item\n")
				else:
					fpw.write("node,adjacency\n")

			for n in range(len(self.data)):
				if alphabet:
					if self.type=="tra":
						ns=str(n)
					else:
						ns=Data._num2alpha(n)
					fpw.write(ns+",")

				ele=[]
				for adj in self.data[n]:
					if alphabet:
						ele.append(Data._num2alpha(adj))
					else:
						ele.append(str(adj))
				fpw.write(" ".join(ele))
				fpw.write("\n")

	def writeNodes(self,oFile,alphabet=False):
		nodes=self.getNodes(alphabet=alphabet)
		with open(oFile,"w") as fpw:
			if alphabet:
				fpw.write("node\n")
			for node in nodes:
				fpw.write(str(node))
				fpw.write("\n")

	def writeEdges(self,oFile,alphabet=False):
		"""
		CSVファイルに出力する。
		form="edge"でノードペアのエッジを一行として出力し、
		form="adjacent"で隣接行列形式で出力する。

		.. code-block:: python
			:linenos:

			>>> dat=Data("numGraph1")
			>>> n,e=dat.getList()
			>>> print(n)
			['0', '1', '2', '3', '4', '5', '6']
			>>> print(e)
			[(0, 1), (0, 2), (0, 4), (1, 2), (1, 4), (1, 6), (2, 3), (2, 6), (3, 4), (4, 5)]
			>>> dat.plot()
			>>> dat.write("edge"," ","g.txt")
			>>> Data.head("g.txt")

		"""

		delim="," if alphabet else " "
		edges=self.getEdges(alphabet=alphabet)
		with open(oFile,"w") as fpw:
			if alphabet:
				if self.type=="tra":
					fpw.write("tid,item\n")
				else:
					fpw.write("node1,node2\n")
			for edge in edges:
				fpw.write(str(edge[0])+delim+str(edge[1]))
				fpw.write("\n")

	def plot(self,alphabet=False,oFile=None):
		if self.type=="tra":
			row_labels=[]
			col_labels=[]
			for tra in range(len(self.data)):
				row_labels.append(str(tra))
			for item in range(len(self.nodes)):
				if alphabet:
					col_labels.append(Data._num2alpha(item))
				else:
					col_labels.append(str(item))

			row_size=len(self.data)
			col_size=len(self.nodes)
			mtx=np.full((row_size,col_size),0)
			for tra in range(len(self.data)):
				for item in self.data[tra]:
					mtx[tra][item]=1

			fig, ax = plt.subplots(figsize=(row_size/3,col_size/3))
			heatmap = ax.pcolor(mtx, cmap=plt.cm.Blues)

			ax.set_xticks(np.arange(mtx.shape[0]) + 0.5, minor=False)
			ax.set_yticks(np.arange(mtx.shape[1]) + 0.5, minor=False)
			ax.invert_yaxis()
			ax.xaxis.tick_top()
			ax.set_xticklabels(col_labels, minor=False)
			ax.set_yticklabels(row_labels, minor=False)
			if oFile==None:
				plt.show()
			else:
				plt.savefig(oFile)

		else:
			g = Graph(edges=self.edges)
			vStyle = {}
			vStyle["vertex_size"] = 25
			vStyle["vertex_color"] = ["white" for gender in self.nodes]
			if alphabet:
				n=[]
				for node in self.nodes:
					n.append(Data._num2alpha(node))
				vStyle["vertex_label"] = n
			else:
				vStyle["vertex_label"] = self.nodes

			vStyle["edge_width"] = [1 for i in self.edges]
			vStyle["layout"] = g.layout("kamada_kawai")
			vStyle["bbox"] = (200, 200)
			vStyle["margin"] = 30
			if oFile==None:
				plot(g,**vStyle)
			else:
				plot(g,oFile,**vStyle)

	def head(fName,top=6):
		try:
			with open(fName) as fpr:
				c=0
				for line in fpr:
					if top==0 or c<top:
						print(line.strip())
						c+=1
					else:
						break
				print("")
		except (RuntimeError, TypeError, NameError):
			pass

	def _graph00(self):
		self.type="graph"
		self.data=[
		  [1,4,2],
		  [2,4,6],
		  [3,5],
		  [4],
		  [5],
		  [],
		  []
		]

	def _graph01(self):
		self.type="graph"
		self.data=[
		  [1,4,2],
		  [4,2],
		  [3],
		  [],
		  [5,3],
		  [2],
		  []
		]

	def _tra00(self):
		self.type="tra"
		self.data=[
			[2,4],
			[3,4,5],
			[0,1,3,5],
			[1,3,5],
			[0,1,3,4],
			[0,1,3,4,5]
		]

	def _numTraDB1w(self,fName="numTraDB1w.csv"):
		oFile="{0}/{1}".format(self.oPath,fName)
		data='''-1
1
-1
1
1
-1
'''
		self._writeFile(data,oFile)
		return oFile


if __name__ == '__main__':

	"""
	dat=Data("tra00")

	tadj=dat.getAdjMatrix()
	tadja=dat.getAdjMatrix(alphabet=True)
	print(tadj)
	print(tadja)
	item=dat.getNodes()
	tras=dat.getEdges()
	print(item)
	print(tras)
	item_a=dat.getNodes(alphabet=True)
	tras_a=dat.getEdges(alphabet=True)
	print(item_a)
	print(tras_a)

	dat.writeAdjMatrix(oFile="xxtadj")
	dat.writeAdjMatrix(oFile="xxtadja",alphabet=True)
	dat.writeNodes(oFile="xxitems")
	dat.writeNodes(oFile="xxitemsa",alphabet=True)
	dat.writeEdges(oFile="xxtras")
	dat.writeEdges(oFile="xxtrasa",alphabet=True)
	Data.head("xxtadj",top=0)
	Data.head("xxtadja",top=0)
	Data.head("xxitems",top=0)
	Data.head("xxitemsa",top=0)
	Data.head("xxtras",top=0)
	Data.head("xxtrasa",top=0)

	dat.plot(oFile="xxtra.png")
	dat.plot(oFile="xxtraa.png",alphabet=True)
	"""


	dat=Data("graph00")

	adj=dat.getAdjMatrix()
	print(adj)
	adja=dat.getAdjMatrix(alphabet=True)
	print(adja)

	node=dat.getNodes()
	edge=dat.getEdges()
	print(node)
	print(edge)
	nodea=dat.getNodes(alphabet=True)
	edgea=dat.getEdges(alphabet=True)
	print(nodea)
	print(edgea)

	dat.writeAdjMatrix(oFile="xxadj")
	dat.writeAdjMatrix(oFile="xxadja",alphabet=True)
	dat.writeNodes(oFile="xxnodes")
	dat.writeNodes(oFile="xxnodesa",alphabet=True)
	dat.writeEdges(oFile="xxedges")
	dat.writeEdges(oFile="xxedgesa",alphabet=True)
	Data.head("xxadj",top=0)
	Data.head("xxadja",top=0)
	Data.head("xxnodes",top=0)
	Data.head("xxnodesa",top=0)
	Data.head("xxedges",top=0)
	Data.head("xxedgesa",top=0)

	dat.plot(oFile="xxg1.png")
	dat.plot(oFile="xxg1a.png",alphabet=True)


