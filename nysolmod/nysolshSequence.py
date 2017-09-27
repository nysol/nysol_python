# -*- coding: utf-8 -*-
import shlex
import re

class NysolShellSequence(object):

	def __init__(self) :
		self.cmdlist=None

	def add(self,obj) :
		if self.cmdlist==None:
			self.cmdlist = obj
		else:
			obj.inp = self.cmdlist
			self.cmdlist = obj


	def show(self) :
		self.cmdlist.show()
		
	def run(self) :
		return self.cmdlist.run()
		
		