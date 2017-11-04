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

		#iolist抽出
		for linno ,cmd  in enumerate(cmdlists):
			cmdptn = cmd[0]
			cmdio = cmd[1]
			# コマンド毎にチェックし方を変えれるようにする
			for k ,v in cmdio.items():

				if k == 'o' or k == 'u':
					if not v in conectLIST:
						conectLIST[v] =[[],[]]
					conectLIST[v][0].append([k,linno])
				else:
					if not v in conectLIST:
						print( k + " model err before output")
					conectLIST[v][1].append([k,linno])


		for key ,val in conectLIST.items():
			if len(val[0]) != 1 or len(val[1]) == 0 :
				print( key + " model err")

		#print self.cmdlist
		#cmd作成
		newcmdlist = []
		interobj = {}
		newruncmd = None

		#ioflg =False
		for linno ,cmd  in enumerate(cmdlists):
			#print linno ,cmd ,newruncmd, interobj
			cmdptn = cmd[0]
			cmdio = cmd[1]
			#print cmdptn ,cmdio

			if newruncmd == None :

				if not "i" in cmdio and len(cmdptn.inplist["i"])==0:
					print(cmdptn.name + " model err (" + str(linno)  + ")"    )

				#newruncmd = copy.deepcopy(cmdptn)
				newruncmd = cmdptn
				if "i" in cmdio:
					newruncmd.paraUpdate({"i":interobj[cmdio['i']]})
					interobj[cmdio['i']].outlist["o"].append(newruncmd)
				if "m" in cmdio:
					newruncmd.paraUpdate({"m":interobj[cmdio['m']]})
					interobj[cmdio['m']].outlist["o"].append(newruncmd)

			else:
				#tmpptn  = copy.deepcopy(cmdptn)
				tmpptn  = cmdptn
				tmpptn.inplist["i"].append(newruncmd)
				newruncmd.outlist["o"].append(cmdptn)
				newruncmd = tmpptn
				if "i" in cmdio:
					newruncmd.paraUpdate({"i":interobj[cmdio['i']]})
					interobj[cmdio['i']].outlist["o"].append(newruncmd)
				if "m" in cmdio:
					newruncmd.paraUpdate({"m":interobj[cmdio['m']]})
					interobj[cmdio['m']].outlist["o"].append(newruncmd)

			if "u" in cmdio:
				interobj[cmdio['u']] = newruncmd

			if "o" in cmdio:
				interobj[cmdio['o']] = newruncmd
				newruncmd =None

			#writescv ,writelistで終了
			if cmdptn.name == "writecsv" or cmdptn.name=="writelist"  :
				newcmdlist.append(newruncmd)
				newruncmd = None

		#print newcmdlist[0]
		#print newcmdlist[0].inplist

		return newcmdlist
#			if cmd[0].name = "readcsv"
			#if ioflg==False and 

		
		
	def run(self,**kwd) :

		runlist = copy.deepcopy(self.cmdlist)
		runcmds = self.makeNetwork(runlist)

		nowMsgFlg = self.msgFlg
		if "msg" in kwd:
			if kwd["msg"] == "on" :
				nowMsgFlg=True

		if nowMsgFlg :
			for runcmd in runcmds:
				runcmd.run(msg="on")
		else:
			for runcmd in runcmds:
				return runcmd.run()


	def drawModel(self , fname=None) :
		oldenv = copy.deepcopy(self.cmdlist)
		runcmds = self.makeNetwork()
		for runcmd in runcmds:
			runcmd.drawModel(fname)
		
		self.cmdlist = oldenv

	def __lshift__(self,obj):
		self.add(obj)
