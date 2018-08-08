# -*- coding: utf-8 -*-
import shlex
import re
import copy

class script(object):

	def __init__(self) :
		self.cmdlist=[]
		self.msgFlg=False

	def add(self,obj,**kwd) :
		self.cmdlist.append([obj,kwd])
		#if self.cmdlist==None:
		#	self.cmdlist = obj
		#else:
		#	obj.inp = self.cmdlist
		#	self.cmdlist = obj

	def msgOn(self):
		self.msgFlg=True

	def msgOff(self):
		self.msgFlg=False

	def show(self) :
		print(self.cmdlist)
		
	def makeNetwork(self,cmdlists):

		conectLIST={}
		#f.w.コマンド毎にする
		ioParams = {"i":0,"m":0,"o":1,"u":1}

		# IO list check
		# f.w.
		# コマンド毎にチェックし方を変えれるようにする 
		# io以外にも拡張 
		for linno ,cmd  in enumerate(cmdlists):
			cmdptn = cmd[0]
			cmdio = cmd[1]
			for k ,v in cmdio.items():
				if not isinstance(v,str):
					continue
				ioTP = ioParams[k]
				if ioTP == 0 :
					if not v in conectLIST:
						print( k + " model err before output")
					conectLIST[v][1].append([k,linno])

				elif ioTP == 1 :
					if not v in conectLIST:
						conectLIST[v] =[[],[]]
					conectLIST[v][0].append([k,linno])

		for key ,val in conectLIST.items():
			if len(val[0]) != 1 or len(val[1]) == 0 :
				print( key + " model err")

		#cmd作成
		newcmdlist = []
		interobj = {}
		newruncmd = None

		for linno ,cmd  in enumerate(cmdlists):
			cmdptn = cmd[0]
			cmdio = cmd[1]

			if newruncmd != None and (not "i" in cmdio or len(cmdio["i"])==0):
					tmpptn  = cmdptn
					tmpptn.inplist["i"].append(newruncmd)
					newruncmd.outlist[newruncmd.nowdir].append(cmdptn)
					newruncmd = tmpptn
			else:
				newruncmd = cmdptn

			for k ,v in cmdio.items():
				if isinstance(v,str):
					ioTP = ioParams[k]
					if ioTP == 0 :
						newruncmd.paraUpdate({k:interobj[v][0]})
						interobj[v][0].outlist[interobj[v][1]].append(newruncmd)
					elif ioTP == 1 :
						interobj[v] = [newruncmd,k]
				else:
					newruncmd.paraUpdate({k:v})

			#writescv ,writelistで終了
			if cmdptn.name == "writecsv" or cmdptn.name=="writelist"  :
				newcmdlist.append(newruncmd)
				newruncmd = None

		return newcmdlist
		
		
	def run(self,**kwd) :

		runlist_org = copy.deepcopy(self.cmdlist)
		runcmds = self.makeNetwork(self.cmdlist)

		nowMsgFlg = self.msgFlg
		if "msg" in kwd:
			if kwd["msg"] == "on" :
				nowMsgFlg=True

		from nysol.mcmd.nysollib.core import runs 

		if nowMsgFlg :
			runs(runcmds,msg="on")
		else:
			runs(runcmds)

		self.cmdlist = runlist_org


	def drawModel(self,fname=None):

		runlist = copy.deepcopy(self.cmdlist)
		runcmds = self.makeNetwork(runlist)

		from nysol.mcmd.nysollib.core import drawModels as drawModels

		drawModels(runcmds,fname)

	def drawModelD3(self,fname=None):

		runlist = copy.deepcopy(self.cmdlist)
		runcmds = self.makeNetwork(runlist)

		from nysol.mcmd.nysollib.core import drawModelsD3 as drawModelsD3

		drawModelsD3(runcmds,fname)


	def modelInfo(self):

		runlist = copy.deepcopy(self.cmdlist)
		runcmds = self.makeNetwork(runlist)

		from nysol.mcmd.nysollib.core import modelInfos as modelInfos

		return modelInfos(runcmds)

	def __lshift__(self,obj):
		self.add(obj)
