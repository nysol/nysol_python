#!/usr/bin/env python
# -*- coding: utf-8 -*-
# debug利用
from pprint import pprint as pprint
import collections
import math
import glob
import heapq
import nysol.mcmd as nm
import os
from nysol.util._utillib import mcsvout as mcsvout
from nysol.util.mtemp import Mtemp
from nysol.util.mmkdir import mkDir


# タプルに格納されたパターンを文字列に変換する
# ex. (0,"A",1,"B",2,"C") => A>BC
def pat2str(pat):
	s=""
	for ele in pat:
		if ele==0: # 先頭token
			pass
		elif ele==1: # sequence token
			s+=">"
		elif ele==2: # element token
			pass
		else: # token以外はitem
			s+=ele
	return s

# RのCSPADEと同様のフォーマットに変換
# ex. (0,"A",1,"B",2,"C") => {A} {B C}
def pat2strR(pat):
	s=[]
	ss=[]
	for ele in pat:
		if ele==0:
			ss=[]
		elif ele==1:
			s.append(ss)
			ss=[]
		elif ele==2:
			pass
		else:
			ss.append(ele)
	s.append(ss)
	sss=[]
	for ele in s:
		sss.append("{"+" ".join(ele)+"}")
	return " ".join(sss)
#pat=(0,"A",1,"B")
#print(pat)
#print(pat2strR(pat))
#pat=(0,"A",1,"B",2,"C")
#print(pat)
#print(pat2strR(pat))
#exit()

# 1行1-itemになるようにフォーマットに変換
# ex. (0,"A",1,"B",2,"C") => [["A"],["B","C"]]
def pat2arr(pat):
	s=[]
	ss=[]
	for ele in pat:
		if ele==0:
			ss=[]
		elif ele==1:
			s.append(ss)
			ss=[]
		elif ele==2:
			pass
		else:
			ss.append(ele)
	s.append(ss)
	return s
#pat=(0,"A",1,"B",2,"C")
#print(pat)
#print(pat2arr(pat))
#exit()


# latticeを構成するノードクラス
class Node:
	def __init__(self,lattice,level,pattern,freq):
		self.lattice=lattice # 親に戻るため
		self.level=level # ボトム=0とした時のレベル k-sequenceのkに該当する
		# sequenceパターン
		# 辞書キーに使うのでタプルで表現する
		# ex. D>BF>A => ("D",1,"B",2,"F",1,"A")
		self.pattern=pattern
		self.frequency=freq # support件数
		self.idList=[] # patternの最終アイテムがマッチする(sid,startEid,eid)リスト:startEidはwin制約計算のため
		self.children=[] # lattice上の次のノード
		self.parent1=None
		self.parent2=None
		self.joinType=None
		self.item2node={} # 1-sequenceでのみ利用
		self.outDoneFlag=False # show関数で表示時に利用するフラグ
		self.maximal=False # maximal=Trueの時のみセットされる
		c=0
		for item in pattern:
			if item==1:
				c+=1
		self.length=c+1

		self.freqOther=0 # 再帰足し算するので0初期化
		self.idListOther=[]
	#def initOther(self,freq)
	#	self.others=Node(None,None,None,freq) # 他クラスのデータを同じ構造で格納

	# self.patternと同じ長さのnode.patternのpfrefixが同じか判定
	# samePrefix(A>B,A>B>C)=True, samePrefix(A>B,A,B>C)=False
	# len(self.pattern)>len(node.pattern)の時はエラー
	def samePrefix(self,node):
		for i,item in enumerate(self.pattern):
			if node.pattern[i]!=item:
				return False
		return True

	def getNodes(self,targetLevel,nodes):
		if self.level==targetLevel:
			nodes.append(self)
		for node in self.children:
			node.getNodes(targetLevel,nodes)

	def postCal(self,tSize,oSize):
		self.pprob=float(self.frequency*tSize)/float(self.frequency*tSize+self.freqOther*oSize)
		for node in self.children:
			node.postCal(tSize,oSize)

	# nodeを深さ優先で探索し、latice上の第level nodeのpatternを表示する
	# level==Noneの場合は、全レベルのpatternを表示する
	# simple=Trueの場合は、パターンとsupportのみ出力
	# simple=Falseの場合は、階層整形,パターン,support,idListを出力
	# redundant=Trueにすると、laticeの接続関係を全出力。
	def show(self,level=None,simple=False,redundant=False):
		if not redundant and self.outDoneFlag:
			return
		if level==None or self.level==level:
			if simple:
				print("%s,%d"%(pat2str(self.pattern),self.frequency))
			else:
				print("  "*self.level, "%s(%d,%d,%d,%d)"%(pat2str(self.pattern),self.level,self.length,self.frequency,self.freqOther),self.idList,self.idListOther)
		self.outDoneFlag=True
		if len(self.children)==0:
			return
		for node in self.children:
			node.show(level,simple,redundant)

	def getMaximalCandidate(self,patterns):
		if len(self.children)==0:
			if self.level not in patterns:
				patterns[self.level]=set()
			patterns[self.level].add(self)
		for node in self.children:
			node.getMaximalCandidate(patterns)

	def clearOutDoneFlag(self):
		self.outDoneFlag=False
		for node in self.children:
			node.clearOutDoneFlag()

	def selTopK(self,oParams,pQue):
		if self.outDoneFlag:
			return
		doOutput=True
		if oParams["maximal"] and not self.maximal:
			doOutput=False
		if oParams["minSize"] and self.level<oParams["minSize"]:
			doOutput=False
		if oParams["minLen"] and self.length<oParams["minLen"]:
			doOutput=False
		if oParams["maxSup"] and self.frequency>oParams["maxSup"]:
			doOutput=False
		if oParams["minPprob"] and self.pprob<oParams["minPprob"]:
			doOutput=False

		if doOutput:
			# nodeをpriority queueに入れる
			node=(self.level,self.length,self.frequency,self.pattern,self)
			if len(pQue)>=oParams["topk"]:
				#print(pQue)
				#print(node)
				heapq.heappushpop(pQue,node)
			else:
				heapq.heappush(pQue,node)
			self.outDoneFlag=True

		# 再帰処理
		for node in self.children:
			node.selTopK(oParams,pQue)

	# pattern全出力(Rの結果との比較のため)
	def outputAll(self,oCSV):
		if self.outDoneFlag:
			return
		#print(self.pattern,pat2str(self.pattern),pat2strR(self.pattern))
		oCSV.write([pat2strR(self.pattern),self.frequency])
		self.outDoneFlag=True
		for node in self.children:
			node.outputAll(oCSV)

def is_subseq(x, y):
    it = iter(y)
    return all(c in it for c in x)

# Latticeクラス
class Lattice:
	def __init__(self,freq,eParams):
		self.tSize=freq # そのクラスのtransactionサイズ
		self.oSize=0    # 他クラスのtransactionサイズ
		self.minSup=eParams["minSup"]
		if self.minSup==None:
			self.minSup=math.ceil(freq*eParams["minSupProb"])
		# latticeボトムノードの登録
		self.root=Node(self,0,(0,),freq)
		self.eParams=eParams

	#def initOther(self,freq):
	#	self.oSize=freq # 他クラスのtransactionサイズ
		#self.root.initOther(freq)

	# 1-sequenceの登録
	# dataはsid+eid(時刻順)+itemでsortingされていることが前提
	def addOnePattern(self,data,eParams,other=False):
		counter = collections.Counter() # 一旦ここに件数カウント結果を入れる
		# sequence
		# [0, [                       # sequence[0]=0(sid)
		#   [0, ["C", "D"]],          # sequence[1]=[0, ["C", "D"]](element)
		#	  [1, ["A", "B", "C"]],     #             eid  item
		#   [2, ["A", "B", "F"]],
		#   [3, ["A", "C", "D", "F"]]
		#  	]
		#	],
		for sequence in data:
			onePattern=set() # sid毎に出現するitemをuniqueにセット
			for element in sequence[1]:
				for item in element[1]:
					onePattern.add(item)
			for item in sorted(list(onePattern)):
				counter[item]+=1

		# カウント結果、頻出な1-sequenceをlatticeに反映させる
		if other:
			for child in self.root.children:
				child.freqOther+=counter[child.pattern[0]]
		else:
			for item,freq in counter.items():
				if freq>=self.minSup:
					self.root.children.append(Node(self,1,(item,),freq))
					self.root.item2node[item]=self.root.children[-1]

	# 2-sequenceの登録
	# dataはsid+eid(時刻順)+itemでsortingされていることが前提
	def addTwoPattern(self,data,eParams,other=False):
		# 頻出1-sequenceを構成するitem一覧
		freqOnePattern=set()
		for node in self.root.children:
			freqOnePattern.add(node.pattern[0])

		counter={} # 一旦ここに件数カウント結果を入れる
		idList={} # 2-sequenceが出現する(sid,eid)ペア(A>BならA,>,BがキーでBの出現位置(sid,eid)のみ保持)
		for sid,sequence in data:
			# sequenceデータを１次元配列に変換 要素は(item,time)
			seq=[]
			for eid,element in sequence:
				for item in element:
					if item in freqOnePattern:
						seq.append((item,eid))

			#print("seq",seq)
			# seq [('D', 0), ('A', 1), ('B', 1), ('A', 2), ('B', 2), ('F', 2), ('A', 3), ('D', 3), ('F', 3)]

			done=set()
			for i in range(len(seq)-1):
				for j in range(i+1,len(seq)):
					item1=seq[i][0]
					time1=seq[i][1]
					item2=seq[j][0]
					time2=seq[j][1]
					token=1
					if eParams["minGap"]!=None and time2-time1<eParams["minGap"]:
						continue
					if eParams["maxGap"]!=None and time2-time1>eParams["maxGap"]:
						continue
					if time1==time2:
						token=2
					if item1 not in counter:
						counter[item1]={}
						idList[item1]={}
					if item2 not in counter[item1]:
						counter[item1][item2]={}
						idList[item1][item2]={}
					if token not in counter[item1][item2]:
						counter[item1][item2][token]=0
						idList[item1][item2][token]=[]
					if item1+item2+str(token) not in done:
						counter[item1][item2][token]+=1
						done.add(item1+item2+str(token))
					#if (sid,time1,time2) not in idList[item1][item2][token]: ##### 遅い
					idList[item1][item2][token].append((sid,time1,time2)) # idListは全てを記録

		# カウント結果、頻出な2-sequenceをlatticeに反映させる
		# Node.freqOtherの更新
		if other:
			for node1 in self.root.children:
				item1=node1.pattern[0]
				for node2 in node1.children:
					item2=node2.pattern[-1]
					token=node2.pattern[-2]
					if item1 in counter and item2 in counter[item1] and token in counter[item1][item2]:
						node2.freqOther+=counter[item1][item2][token]
						node2.idListOther+=idList[item1][item2][token]
					#else:
					#	node2.freqOther=0
					#	node2.idListOther=[]

		# 頻出な2-sequenceの件数(Node.frequency)の更新
		else:
			for item1 in counter.keys():
				node1=self.root.item2node[item1]
				for item2 in counter[item1].keys():
					node2=self.root.item2node[item2]
					for token,freq in counter[item1][item2].items():
						if freq>=self.minSup:
							joinedPattern=(node1.pattern[0],token,node2.pattern[0])
							newNode=Node(self,node1.level+1,joinedPattern,freq)
							if newNode.length<=eParams["maxLen"]:
								newNode.idList=idList[item1][item2][token]
								#newNode.parent1=node1
								#newNode.parent2=node2
								# level1 => level2リンク
								node1.children.append(newNode)

	# サイズの等しい2つのk-sequenceから(k+1)-sequenceを結合(v)作成する(論文p.48)
	# 結合方式(e:event join,s:sequence join),2つのatom,結合されたpatternの３つのリストを返す
	# 2つのシーケンスは(k-1)-prefixが同じでなければならない。<=prefix同クラス
	# Pは任意のシーケンス ex. P=X>YZなら PA=X>YZA, P>A=X>YZ>A
	# ee型: v(eventAtom,eventAtom) ex. PA,PB => PAB
	# es型: v(eventAtom,sequenceAtom) ex. PA,P>B =>PA>B
	# ss型: v(seqAtom1,seqAtom2) ex. P>A,P>B =>P>AB, P>A>B, P>B>A
	# 同一ss型: v(seqAtom1,seqAtom1) ex. P>A,P>A =>P>A>A
	def joinAtoms(self,atom1,atom2):
		candidates=[]
		if atom1.pattern[-2]==2:
			if atom2.pattern[-2]==2:
				if atom1.pattern[-1]!=atom2.pattern[-1]: #同じpatternは同じeventには入れない
					# ee型
					# PA v PB = PAB
					pattern=atom1.pattern
					pattern+=(2,atom2.pattern[-1])
					candidates.append(["e",atom1,atom2,pattern])
			else:
				# es型(atom1がeの場合)
				# PA v P>B = PA>B
				pattern=atom1.pattern
				pattern+=(1,atom2.pattern[-1])
				candidates.append(["s",atom1,atom2,pattern])
		else:
			if atom2.pattern[-2]==2:
				# es型(atom2がeの場合)
				# P>A v PB = PB>A
				pattern=atom2.pattern
				pattern+=(1,atom1.pattern[-1])
				candidates.append(["s",atom2,atom1,pattern])
			else:
				if atom1.pattern[-1]==atom2.pattern[-1]:
					# 同一es型
					# P>A v P>A = P>A>A
					pattern=atom1.pattern
					pattern+=(1,atom1.pattern[-1])
					candidates.append(["s",atom1,atom2,pattern])
				else:
					# es型
					# P>A v P>B = P>A>B
					pattern=atom1.pattern
					pattern+=(1,atom2.pattern[-1])
					candidates.append(["s",atom1,atom2,pattern])
					# P>A v P>B = P>B>A
					pattern=atom2.pattern
					pattern+=(1,atom1.pattern[-1])
					candidates.append(["s",atom2,atom1,pattern])
					# P>A v P>B = P>AB
					pattern=atom1.pattern
					pattern+=(2,atom2.pattern[-1])
					candidates.append(["e",atom1,atom2,pattern])
		return candidates

	# 2つのatomをsupportするidListを時間joinする
	# other=Trueの時は、idListOtherに基づいてjoin
	def temporalJoin(self,candidate,other=False):
		joinType=candidate[0]
		atom1=candidate[1]
		atom2=candidate[2]
		joinedPattern=candidate[3]
		#print("### temporalJoin",pat2str(atom1.pattern),"V",pat2str(atom2.pattern),"=",pat2str(joinedPattern))
		#print("### temporalJoin",atom1.pattern,"V",atom2.pattern,"=",joinedPattern)
		#print("atom1=",atom1,atom1.idList)
		#print("atom2=",atom2,atom2.idList)

		idList=[]
		i1=0
		i2=0
		if other:
			len1=len(atom1.idListOther)
			len2=len(atom2.idListOther)
		else:
			len1=len(atom1.idList)
			len2=len(atom2.idList)
		while i1<len1 and i2<len2:
			# sidが一致するまでお互い飛ばす
			if other:
				sid1=atom1.idListOther[i1][0]
				sid2=atom2.idListOther[i2][0]
			else:
				sid1=atom1.idList[i1][0]
				sid2=atom2.idList[i2][0]
			if sid1<sid2:
				i1+=1
				continue
			if sid1>sid2:
				i2+=1
				continue

			# sidが一致する中でのeidの大小関係比較
			if other:
				eid1=atom1.idListOther[i1][2]
				eid2=atom2.idListOther[i2][2]
			else:
				eid1=atom1.idList[i1][2]
				eid2=atom2.idList[i2][2]

			if joinType=="s": # sequence結合
				if eid1>=eid2:
					i2+=1
					continue
				else:
					if self.eParams["minGap"]==None or eid2-eid1>=self.eParams["minGap"]:
						if self.eParams["maxGap"]==None or eid2-eid1<=self.eParams["maxGap"]:
							if other:
								startEid=atom1.idListOther[i1][1] # 先頭マッチ時刻,atom2でも同じ
							else:
								startEid=atom1.idList[i1][1] # 先頭マッチ時刻,atom2でも同じ
							#print("####",eParams["maxWin"],">=",eid2-startEid+1,eid2,startEid)
							if self.eParams["maxWin"]==None or eid2-startEid+1<=self.eParams["maxWin"]:
								if (sid1,startEid,eid2) not in idList: ##### 遅い(なくても動く)
									idList.append((sid1,startEid,eid2)) # sid1はsid2でも同じ
					#i1+=1
					i2+=1
			else:             # element結合(joinType=="e") gapのチェック必要なし
				if eid1<eid2:
					i1+=1
					continue
				elif eid1>eid2:
					i2+=1
					continue
				else: #eid1==eid2
					if other:
						startEid=atom1.idListOther[i1][1] # 先頭マッチ時刻,atom2でも同じ
					else:
						startEid=atom1.idList[i1][1] # 先頭マッチ時刻,atom2でも同じ
					if self.eParams["maxWin"]==None or eid1-startEid+1<=self.eParams["maxWin"]:
						idList.append((sid1,startEid,eid1)) # sid1,eid1はsid2,eid2でも同じ
					i1+=1
					i2+=1
		#print("joined idList=",idList)
		return idList

	# idList のsidのunique数をカウントする
	def support(self,idList):
		counter=set()
		for line in idList:
			counter.add(line[0])
		return len(counter)

	# nodeと同じpatternを(k-1)-prefixに持つ子ノードを探索
	def samePrefixNodes(self,node):
		sameNodes=[]
		for child in node.children:
			# print("spn=",node,child)
			if node.samePrefix(child):
				sameNodes.append(child)
		return sameNodes

	# atomsにはprefixを共有するsequenceが格納されている
	def enum_sub(self,atoms,eParams):
		#print("&&atoms=",[pat2str(atom.pattern) for atom in atoms])
		freqPatterns=[] # prefixが同じpatternリストを格納する2次元配列
		for i in range(len(atoms)-1):
			atom1=atoms[i]
			#print("############1",atom1.item)
			for j in range(i,len(atoms)):
				atom2=atoms[j]
				#print(atom1.item,atom2.item)
				# ２つのk0sequence atom1,atom2から結合される(k+1)-sequenceの候補を作成
				candidates=self.joinAtoms(atom1,atom2)
				# それらの候補のidListを時間joinし、support件数をカウントする
				# print("candidates",candidates)
				for candidate in candidates:
					#print(candidate)
					# atom1,atom2をtemporal joinしてcandidateのidListを作成
					idList=self.temporalJoin(candidate)
					freq=self.support(idList)

					if freq>=self.minSup:
						newNode=Node(self,atom1.level+1,candidate[3],freq)
						if newNode.length<=eParams["maxLen"]:
							newNode.idList=idList
							newNode.joinType=candidate[0]
							newNode.parent1=candidate[1]
							newNode.parent2=candidate[2]
							atom1.children.append(newNode)
							if atom1!=atom2: # P>A V P>Aの場合はatom1だけに登録
								atom2.children.append(newNode)
							freqPatterns.append(newNode)

		if len(freqPatterns)>0:
			# print("###level=",atoms[0].level,eParams["maxSize"])
			if atoms[0].level+1<eParams["maxSize"]:
				for atom in atoms:
					nextAtoms=self.samePrefixNodes(atom)
					# print("xx1",[pat2str(atom.pattern) for atom in nextAtoms])
					#for add in nextAtoms:
					#	print("add=",add)
					self.enum_sub(nextAtoms,eParams)

	# 3-sequence以上のsequenceの列挙
	# 2つのprefix同クラスのk-sequenceから(k+1)-sequenceを結合(join:v)する。
	def enum(self,eParams):
		for node in self.root.children:
			# print(node.pattern)
			# 一つの1-sequenceをbottomとしたprefix同クラスのatomsを与えて再帰処理start
			atoms=self.samePrefixNodes(node)
			#print("xx1",[pat2str(atom.pattern) for atom in atoms])
			self.enum_sub(atoms,eParams)
			#return

	# targetLevelのnodeリストを返す
	def getNodes(self,targetLevel):
		nodes=[]
		self.root.getNodes(targetLevel,nodes)
		return nodes

	# 3-sequence以上のother dataの件数カウント
	# 子から2人の親のother dataのidListをtemporal joinすることでカウント
	def countOther(self,eParams):
		targetLevel=3
		while True:
			nodes=self.getNodes(targetLevel)
			if len(nodes)==0:
				break
			for node in nodes:
				# nodeはparent1,paret2のjoinでできた
				candidate=[node.joinType,node.parent1,node.parent2,node.pattern]
				idList=self.temporalJoin(candidate,other=True)
				node.freqOther+=self.support(idList)
				node.idListOther+=idList
			targetLevel+=1

	def show(self,level=None,simple=False,redundant=False):
		print("### Lattice")
		print("# 括弧内の数字は左からlevel,length,frequency,freqOther")
		self.root.show(level=level,simple=simple,redundant=redundant)
		self.clearOutDoneFlag()

	def postCal(self):
		tSize=self.root.frequency
		oSize=self.root.freqOther
		self.root.postCal(tSize,oSize)

	def setMaximalFlag(self):
		allNodes={} # level別にmaximal候補を獲得
		seqString={} # level別にpattern文字列を格納(tokenはカンマ)
		self.root.getMaximalCandidate(allNodes)
		# seqStringをセット
		levels=sorted(list(allNodes.keys()))
		maxLevel=max(levels)
		minLevel=min(levels)
		for level in levels:
			patterns=""
			for node in allNodes[level]:
				patterns+=pat2str(node.pattern)+","
			# print("patterns[%d]"%level,patterns)
			seqString[level]=patterns

		# k-sequenceが(k+1)-sequenceのsub-sequenceかどうかのチェック
		for i in range(minLevel,maxLevel): # maximalチェック対象レベル(maxLevelそのものはmaximal)
			for node in allNodes[i]:
				# print("checking node[%d]="%i,pat2str(node.pattern))
				for j in range(i+1,maxLevel+1):
					# print("j=",j,is_subseq(pat2str(node.pattern),seqString[j]))
					if not is_subseq(pat2str(node.pattern),seqString[j]):
						node.maximal=True
						# print("maximal=",pat2str(node.pattern))
						break
		#maxLevelは全てmaximal
		for node in allNodes[maxLevel]:
			node.maximal=True

	def clearOutDoneFlag(self):
		self.root.clearOutDoneFlag()

	def selTopK(self,oParams,pQue):
		self.root.selTopK(oParams,pQue)
		self.clearOutDoneFlag()

	def outputAll(self,oFile):
		# print("pattern,frequency")
		with mcsvout(f="pattern,frequency",o=oFile) as oCSV:
			self.root.outputAll(oCSV)

		self.clearOutDoneFlag()

def _readCSV_sub(iFile):
	data=[]
	for block in nm.mcat(i=iFile).keyblock(k="sid",s="sid,eid%n,item",dtype={"sid":"str","eid":"int","item":"str"},otype="dict"):
		# print(block)
		sid=block["sid"][0]
		seq=[]
		elements=[]
		element=[]
		eidPrev=block["eid"][0]
		#print(block["eid"])
		#print(block["item"])
		for i in range(len(block["eid"])):
			eid =block["eid"][i]
			item=block["item"][i]
			# print(sid,eid,item)
			if eid!=eidPrev:
				#print("break")
				elements.append([eidPrev,element])
				element=[]
			element.append(item)
			eidPrev=eid
		elements.append([eidPrev,element])

		#print(sid,eid,item)
		data.append([sid,elements])
	return data

# CSVデータを読み込みlistデータに格納
# classが指定されている場合は、datas[class名]に各クラスのデータが格納される。
# classが指定されていない場合は、datas["single"]にデータが格納される。
def readCSV(iParams):
	#data=readCSVwc(self.iParams["iFile"],self.iParams["iNames"],"xxdataR")
	iFile=iParams["iFile"]
	sidF=iParams["iNames"][0]
	eidF=iParams["iNames"][1]
	itemF=iParams["iNames"][2]
	flds="%s,%s,%s"%(sidF,eidF,itemF)

	temp=Mtemp()
	xxdatPath=temp.file()
	# classファイルの処理
	cFile=iParams["cFile"]
	if cFile!=None:
		csidF=iParams["cNames"][0]
		classF=iParams["cNames"][1]

		temp=Mtemp()
		xxdatPath=temp.file()
		f=None
		f<<=nm.mcut(f="%s:sid,%s:eid,%s:item"%(sidF,eidF,itemF),i=iFile)
		f<<=nm.mdelnull(f="sid,eid,item")
		f<<=nm.muniq(k="sid,eid,item")
		f<<=nm.mjoin(k="sid",K=csidF,m=cFile,f="%s:class"%(classF))
		f<<=nm.msep(s="sid,eid%n,item",d="%s/${class}"%(xxdatPath),p=True)
		f.run()
		classNames=glob.glob("%s/*"%(xxdatPath))
		classNames=[os.path.basename(path) for path in classNames]
		#print(classNames)	
	else:
		mkDir(xxdatPath)
		f=None
		f<<=nm.mcut(f="%s:sid,%s:eid,%s:item"%(sidF,eidF,itemF),i=iFile)
		f<<=nm.mdelnull(f="sid,eid,item")
		f<<=nm.muniq(k="sid,eid,item")
		f<<=nm.msortf(f="sid,eid%n,item",o="%s/single"%(xxdatPath))
		f.run()
		classNames=["single"]

	datas={}
	for name in classNames:
		dataFile="%s/%s"%(xxdatPath,name)
		#os.system("cat %s/%s"%(xxdatPath,name))
		datas[name]=_readCSV_sub(dataFile)
		#print(datas[name])
		#exit()
	return datas

'''
def writeRdata(data,roFile):
	# R用データファイルの出力(sid,eid,サイズ,items)
	# 1 10 2 C D
	# 1 15 3 A B C
	# 1 20 3 A B F
	if roFile!=None:
		with open(roFile,"w") as fpw:
			for i in range(len(data)):
				sid=data[i][0]
				elements=data[i][1]
				for j in range(len(elements)):
					eid=elements[j][0]
					element=elements[j][1]
					#print(element)
					#print(sid,eid,len(element)," ".join(element))
					fpw.write("%s %d %d %s\n"%(sid,eid,len(element)," ".join(element)))
'''

def diff(patR,patP):
	f=None
	f<<=nm.mselstr(f="pattern",v="{}",r=True,i=patP)
	f<<=nm.msortf(f="pattern,frequency",o="xxp")
	f.run()
	f=None
	f<<=nm.mselstr(f="pattern",v="{}",r=True,i=patR)
	f<<=nm.msortf(f="pattern,frequency",o="xxr")
	f.run()
	# print("### diff xxp xxr")
	os.system("diff xxp xxr")

class Spade:

	def checkParams(self):
		if self.eParams["minSup"]==None and self.eParams["minSupProb"]==None:
			raise ValueError("either minSup or minSupProb should be specified.")
		if self.eParams["minSup"]!=None and self.eParams["minSup"]<1:
			raise ValueError("minSup must be equal or greater than 1.")
		if self.eParams["minSupProb"]!=None and (self.eParams["minSupProb"]<0.0 or self.eParams["minSupProb"]>1.0):
			raise ValueError("minSupRation should be in the range 0.0-1.0.")

	def __init__(self,iParams,eParams,oParams):
		self.iParams=iParams
		self.eParams=eParams
		self.oParams=oParams
		self.checkParams()


	# nodeのsequence patternをリストの出力形式に格納し返す
	# ptas: patterns
	#    name     pid eid item
	# [['single', 0, 0, 'D'],
	#  ['single', 0, 1, 'C'],
	#  ['single', 0, 2, 'F'],
	#  ['single', 0, 2, 'D'],  # D>C>FD
	#  ['single', 1, 0, 'D'],
	#             :
	#  
	# stats: patternの統計
	#    name     pid size len freq
	# [['single', 0, 4, 3, 2],
	#  ['single', 1, 4, 3, 2],
	#  ['single', 2, 4, 3, 2],
	#  ['single', 3, 4, 3, 2],
	#  ['single', 4, 4, 3, 2]],
	#
	# occs: 出現
	#    name     pid sid
	# [['single', 0, 'i1'],
	#  ['single', 0, 'i5'],
	#  ['single', 1, 'i5'],
	#  ['single', 1, 'i8'],
	#            :
	def dump(self,name,nodes,oParams):
		pats=[]
		stats=[]
		occs=[]
		for node in nodes:
			patArr=pat2arr(node.pattern)
			# print(patArr)
			# [['D'], ['C'], ['F', 'D']]
			stats.append([name,self.pid,node.level,node.length,node.frequency,node.freqOther,node.pprob])
			for eid,element in enumerate(patArr):
				for item in element:
					pats.append([name,self.pid,eid,item])
			if oParams["oOccs"]!=None:
				sidPrev=None
				for line in node.idList:
					sid=line[0]
					if sidPrev==sid:
						continue
					occs.append([name,self.pid,sid])
					sidPrev=sid
			self.pid+=1
		return (pats,stats,occs)

	def writeCSV(self,rules,oParams):
		# 出力
		if "rule" in self.oParams:
			pass
		else:
			oCSVpats=None
			oCSVstats=None
			oCSVoccs=None
			if oParams["oPats"]!=None:
				oCSVpats=mcsvout(f="class,pid,eid,item",o=oParams["oPats"])
			if oParams["oStats"]!=None:
				oCSVstats=mcsvout(f="class,pid,size,length,frequency,freqOther,pprob",o=oParams["oStats"])
			if oParams["oOccs"]!=None:
				oCSVoccs=mcsvout(f="class,pid,sid",o=oParams["oOccs"])

			for eachRule in rules.values():
				if oCSVpats!=None:
					for line in eachRule[0]:
						oCSVpats.write(line)
				if oCSVstats!=None:
					for line in eachRule[1]:
						oCSVstats.write(line)
				if oCSVoccs!=None:
					for line in eachRule[2]:
						oCSVoccs.write(line)

			if oCSVpats!=None:
				oCSVpats.close()
			if oCSVstats!=None:
				oCSVstats.close()
			if oCSVoccs!=None:
				oCSVoccs.close()

	def run(self):
		###### データのセット
		# listデータ(優先)
		#if self.iParams["iData"]!=None:

		if "iData" in self.iParams:
			datas=self.iParams["iData"]

		# CSVデータ
		elif self.iParams["iFile"]!=None:
			if os.path.exists(self.iParams["iFile"]):
				datas=readCSV(self.iParams)
			else:
				raise ValueError("file not found: %s"%self.iParams["iFile"])
		else:
			raise ValueError("either iData or iFile should be specified for input data.")

		###### 列挙
		lattice={}
		for tName in datas.keys():
			# 対象クラスのsequence列挙
			lattice[tName]=Lattice(len(datas[tName]),self.eParams)
			lattice[tName].addOnePattern(datas[tName],self.eParams)
			if self.eParams["maxSize"]>=2:
				lattice[tName].addTwoPattern(datas[tName],self.eParams)
				if self.eParams["maxSize"]>=3:
					lattice[tName].enum(self.eParams)
			#lattice[tName].show()
			#exit()

			# 対象クラス以外のクラスのsequence列挙
			for oName in datas.keys():
				if tName==oName:
					continue
				lattice[tName].root.freqOther+=len(datas[oName])
				lattice[tName].addOnePattern(datas[oName],self.eParams,other=True)
				if self.eParams["maxSize"]>=2:
					lattice[tName].addTwoPattern(datas[oName],self.eParams,other=True)
					if self.eParams["maxSize"]>=3:
						lattice[tName].countOther(self.eParams)
			#lattice[tName].show()

			# 列挙後の計算
			lattice[tName].postCal()
			lattice[tName].setMaximalFlag()

		###### 出力ルールのセット
		rules={}
		self.pid=0
		for name in lattice.keys():
			#lattice[name].show()
			#lattice[name].show(simple=False,redundant=True)
			#exit()
			pQue=[] # priority queue
			lattice[name].selTopK(self.oParams,pQue)
			nodes=[que[-1] for que in pQue] # que[-1]がlatticeのnode
			# ルールをpats,stats,occsの3つのリストでdump
			rules[name]=self.dump(name,nodes,self.oParams)

		# ファイル出力(rulesはRも同じフォーマット)
		self.writeCSV(rules,self.oParams)

		return rules

		#lattice.calRules()
		#lattice.writeCSVrules(oParams,"xxoutPats","xxoutStats","xxoutIDs")

if __name__ == '__main__':
	data = [
	[1,10,[3,4]],
	[1,15,[1,2,3]],
	[1,20,[1,2,6]],
	[1,25,[1,3,4,6]],
	[2,15,[1,2,6]],
	[2,20,[5]],
	[3,10,[1,2,6]],
	[4,10,[4,7,8]],
	[4,20,[2,6]],
	[4,25,[1,7,8]]
]

#'''
	data = [
    [1, [
			    [10, ["C", "D"]],
			    [15, ["A", "B", "C"]],
          [20, ["A", "B", "F"]],
          [25, ["A", "C", "D", "F"]]
				]
			],
    [2, [
			    [15, ["A", "B", "F"]],
          [20, ["E"]]
				]
			],
    [3, [
			    [10, ["A", "B", "F"]]
			  ]
			],
    [4, [
			    [10, ["D", "G", "H"]],
          [20, ["B", "F"]],
          [25, ["A", "G", "H"]]
				]
			]
]
	#'''
	data = [
    ["1", [
			    [1, ["C", "D"]],
			    [2, ["A", "B", "C"]],
          [3, ["A", "B", "F"]],
          [4, ["A", "C", "D", "F"]]
				]
			],
    ["2", [
			    [1, ["A", "B", "F"]],
          [2, ["E"]]
				]
			],
    ["3", [
			    [1, ["A", "B", "F"]]
			  ]
			],
    ["4", [
			    [1, ["D", "G", "H"]],
          [2, ["B", "F"]],
          [3, ["A", "G", "H"]]
				]
			]
]

	dataOther = [
    ["5", [
			    [1, ["C", "D"]],
			    [2, ["B", "C", "D"]],
          [3, ["A", "B", "F"]],
          [4, ["B", "C", "D", "F"]]
				]
			],
    ["6", [
			    [1, ["A", "B", "F"]],
          [2, ["E"]]
				]
			],
    ["7", [
			    [1, ["B", "C", "D"]]
			  ]
			],
    ["8", [
			    [1, ["D", "G", "H"]],
          [2, ["C", "D"]],
          [3, ["D", "G", "H"]]
				]
			]
]

	datas={"c1":data,"c2":dataOther}


	# 入力パラメータ
	iParams={
		"iFile":"data/zakic.csv",      # トランザクションファイル
		#"iFile":"data/zaki0.csv",
		"iNames":["sid","eid","item"],  # 項目名:左からtraID,time,itemの項目名を指定する(time項目は整数値でなければならない)
		"cFile":"data/zakic_class.csv", # クラスファイル(Noneにすればクラス考慮しない)
		"cNames":["sid","class"],       # 項目名:左からtraID,クラス項目名を指定する
		#"cFile":None,
		#"cNames":None,
		#"iData":datas
		"iData":None                    # リストによる入力データ(これが指定されたら上記ファイル指定は無視される)
	}

	# 列挙パラメータ
	eParams={
		"useR":True,       # Rのcspadeを使って計算する(class指定は使えない)
		"minSup":2,        # 最小サポート(件数)
		"minSupProb":None, # 最小サポート(確率) minSupがNoneでなければminSup優先
		"maxSize":4,       # パターンを構成する総アイテム数の上限
		"maxLen":3,        # パターンを構成する総エレメント数の上限
		"minGap":None,     # エレメント間最小ギャップ長
		"maxGap":None,     # エレメント間最大ギャップ長
		"maxWin":4         # 全体のマッチ幅最大長
	}

	# 出力パラメータ
	oParams={
		"rLog":"xxRlog",     # useR時のRの実行ログファイル
		"rule":False,        # 未実装
		"maximal":False,     # 極大パターンのみ選択
		"topk":20,           # 上位ルール
		"minSize":None,      # maxSizeの最小版
		"minLen":None,       # maxLenの最小版
		"maxSup":None,       # minSupの最大版
		"minPprob":0.7,      # 最小事後確率
		"oPats":"xxoutPats",   # パターン出力ファイル(Noneで出力しない)
		"oStats":"xxoutStats", # 統計量出力ファイル(Noneで出力しない)
		"oOccs":"xxoutOccs"    # 出現出力ファイル(Noneで出力しない)
	}

	spade=Spade(iParams,eParams,oParams)
	rules=spade.run()
	print(rules)


