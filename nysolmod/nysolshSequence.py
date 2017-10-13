# -*- coding: utf-8 -*-
import shlex
import re

class NysolShellSequence(object):

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
		
	def makeNetwork(self):
		runcmd = None
		ioflg =False

		for cmd in self.cmdlist:
			cmdptn = cmd[0]
			cmdio = cmd[1]
			print cmd[0].name
			if cmd[0].name = "readcsv" 
			#if ioflg==False and 
	



	def run(self,**kwd) :
		runcmd = self.makeNetwork()
		return None
		if "msg" in kwd:
			if kwd["msg"] == "on" :
				self.msgFlg=True

		if self.msgFlg :
			return runcmd.run(msg="on")
		else:
			return runcmd.run()

	def __lshift__(self,obj):
		self.add(obj)
