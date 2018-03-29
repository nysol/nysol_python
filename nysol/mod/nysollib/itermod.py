# -*- coding: utf-8 -*-

import copy
import nysol._nysolshell_core as n_core


class Nysol_MeachIter(object):

	def __init__(self,obj):
		

		if len(obj.outlist["o"])!=0:
			print ("type ERORR")
			return None
			
		runobj = copy.deepcopy(obj)
		
		runobj.change_modNetwork()

		uniqmod={} 
		sumiobj= set([])
		runobj.selectUniqMod(sumiobj,uniqmod)

		modlist=[None]*len(uniqmod) #[[name,para]]
		iolist=[None]*len(uniqmod) #[[iNo],[mNo],[oNo],[uNo]]
		runobj.makeModList(uniqmod,modlist,iolist)

		linklist=[]
		runobj.makeLinkList(iolist,linklist)
		# kgshell stock
		self.shobj = n_core.init(runobj.msg)
		self.csvin = n_core.runiter(self.shobj,modlist,linklist)

	def next(self):
		line = n_core.readline(self.csvin)
		if line: 
			return line
		raise StopIteration()


	def __next__(self):
		line = n_core.readline(self.csvin)
		if line: 
			return line
		raise StopIteration()



class Nysol_MeachKeyIter(object):

	def __init__(self,obj,keys,skeys=None):

		if len(obj.outlist["o"])!=0:
			print ("type ERORR")
			return None

		if isinstance(keys,str) :
			newkeys = keys.split(",") 
		elif isinstance(keys,list) :
			newkeys = keys
		else:
			raise Exception("unsuport TYPE")

		dupobj = copy.deepcopy(obj)

		from nysol.mod.submod.msortf import Nysol_Msortf as msortf
		sortkeys = copy.deepcopy(newkeys)
		if skeys != None:
			if isinstance(keys,str) :
				sortkeys.extend(skeys.split(","))
			elif isinstance(keys,list) :
				sortkeys.extend(skeys)
			else:
				raise Exception("unsuport TYPE")
			
		runobj = msortf({"f":sortkeys}).addPre(dupobj)

		runobj.change_modNetwork()

		uniqmod={} 
		sumiobj= set([])
		runobj.selectUniqMod(sumiobj,uniqmod)

		modlist=[None]*len(uniqmod) #[[name,para]]
		iolist=[None]*len(uniqmod) #[[iNo],[mNo],[oNo],[uNo]]
		runobj.makeModList(uniqmod,modlist,iolist)

		linklist=[]
		runobj.makeLinkList(iolist,linklist)
		# kgshell stock
		self.shobj = n_core.init(runobj.msg)
		self.csvin = n_core.runkeyiter(self.shobj,modlist,linklist,newkeys)

	def next(self):
		line = n_core.readkeyline(self.csvin)
		if line: 
			return line
		raise StopIteration()


	def __next__(self):
		line = n_core.readkeyline(self.csvin)
		if line: 
			return line
		raise StopIteration()


class Nysol_MeachKeyDictIter(object):

	def __init__(self,obj,keys,skeys=None):

		if len(obj.outlist["o"])!=0:
			print ("type ERORR")
			return None

		if isinstance(keys,str) :
			newkeys = keys.split(",") 
		elif isinstance(keys,list) :
			newkeys = keys
		else:
			raise Exception("unsuport TYPE")

		dupobj = copy.deepcopy(obj)

		from nysol.mod.submod.msortf import Nysol_Msortf as msortf
		sortkeys = copy.deepcopy(newkeys)
		if skeys != None:
			if isinstance(keys,str) :
				sortkeys.extend(skeys.split(","))
			elif isinstance(keys,list) :
				sortkeys.extend(skeys)
			else:
				raise Exception("unsuport TYPE")
			
		runobj = msortf({"f":sortkeys}).addPre(dupobj)
		runobj.change_modNetwork()

		uniqmod={} 
		sumiobj= set([])
		runobj.selectUniqMod(sumiobj,uniqmod)

		modlist=[None]*len(uniqmod) #[[name,para]]
		iolist=[None]*len(uniqmod) #[[iNo],[mNo],[oNo],[uNo]]
		runobj.makeModList(uniqmod,modlist,iolist)

		linklist=[]
		runobj.makeLinkList(iolist,linklist)
		# kgshell stock
		self.shobj = n_core.init(runobj.msg)
		self.csvin = n_core.runkeyiter(self.shobj,modlist,linklist,newkeys)

	def next(self):
		line = n_core.readkeylineDict(self.csvin)
		if line: 
			return line
		raise StopIteration()


	def __next__(self):
		line = n_core.readkeylineDict(self.csvin)
		if line: 
			return line
		raise StopIteration()



class Nysol_MeachKeyIterWithFlag(object):

	def __init__(self,obj,keys,skeys=None):

		if len(obj.outlist["o"])!=0:
			print ("type ERORR")
			return None

		
		if isinstance(keys,str) :
			newkeys = keys.split(",") 
		elif isinstance(keys,list) :
			newkeys = keys
		else:
			raise Exception("unsuport TYPE")

		
		dupobj = copy.deepcopy(obj)

		from nysol.mod.submod.msortf import Nysol_Msortf as msortf
		sortkeys = copy.deepcopy(newkeys)
		if skeys != None:
			if isinstance(keys,str) :
				sortkeys.extend(skeys.split(","))
			elif isinstance(keys,list) :
				sortkeys.extend(skeys)
			else:
				raise Exception("unsuport TYPE")
			
		runobj = msortf({"f":sortkeys}).addPre(dupobj)

		runobj.change_modNetwork()

		uniqmod={} 
		sumiobj= set([])
		runobj.selectUniqMod(sumiobj,uniqmod)

		modlist=[None]*len(uniqmod) #[[name,para]]
		iolist=[None]*len(uniqmod) #[[iNo],[mNo],[oNo],[uNo]]
		runobj.makeModList(uniqmod,modlist,iolist)

		linklist=[]
		runobj.makeLinkList(iolist,linklist)
		# kgshell stock
		self.shobj = n_core.init(runobj.msg)
		self.csvin = n_core.runkeyiter(self.shobj,modlist,linklist,newkeys)
		self.breakPre = True
	
	
	def next(self):
		data = n_core.readkeylineWithFlag(self.csvin)
		if data: 
			breakTop = self.breakPre
			self.breakPre = data[1]
			return data[0],breakTop,data[1]

		raise StopIteration()


	def __next__(self):

		data = n_core.readkeylineWithFlag(self.csvin)

		if data: 
			breakTop = self.breakPre
			self.breakPre = data[1]
			return data[0],breakTop,data[1]

		raise StopIteration()

