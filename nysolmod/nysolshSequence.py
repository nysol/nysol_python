# -*- coding: utf-8 -*-
import shlex
import re

class NysolShellSequence(object):

	def __init__(self) :
		self.cmdlist=None
		self.msgFlg=False

	def add(self,obj) :
		if self.cmdlist==None:
			self.cmdlist = obj
		else:
			obj.inp = self.cmdlist
			self.cmdlist = obj

	def msgOn(self):
		self.msgFlg=True

	def msgOff(self):
		self.msgFlg=False

	def show(self) :
		self.cmdlist.show()
		
	def run(self,**kwd) :
		if "msg" in kwd:
			if kwd["msg"] == "on" :
				self.msgFlg=True

		if self.msgFlg :
			return self.cmdlist.run(msg="on")
		else:
			return self.cmdlist.run()
