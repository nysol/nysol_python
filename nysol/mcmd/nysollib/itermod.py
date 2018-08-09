# -*- coding: utf-8 -*-
import copy
import nysol._nysolshell_core as n_core

class LineListIter(object):

	def __init__(self,obj,dtype=None,skeys=None):

		if len(obj.outlist["o"])!=0:
			raise Exception("Do not specify last output ")
			return None

		dupobj = copy.deepcopy(obj)

		from nysol.mcmd.submod.msortchk import Nysol_Msortchk as msortchk
		if skeys != None:
			runobj = msortchk({"k":skeys}).addPre(dupobj)
		else :
			runobj = dupobj
				
		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)


		# kgshell stock
		self.shobj = n_core.init(runobj.msg,runobj.runlimit)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None

		self.dptn  = n_core.fldtp(self.csvin,dtype)


	def __nextCore(self):
		line = n_core.getLineList(self.csvin,self.dptn)
		if line: 
			return line

		n_core.close(self.csvin)
		n_core.cancel(self.shobj)
		raise StopIteration()
	
	

	def next(self):

		return self.__nextCore()

	def __next__(self):

		return self.__nextCore()


class LineDictIter(object):

	def __init__(self,obj,dtype=None,skeys=None):
		

		if len(obj.outlist["o"])!=0:
			raise Exception("Do not specify last output ")
			return None
			
		dupobj = copy.deepcopy(obj)

		from nysol.mcmd.submod.msortchk import Nysol_Msortchk as msortchk
		if skeys != None:
			runobj = msortchk({"k":skeys}).addPre(dupobj)
		else :
			runobj = dupobj

		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)

		# kgshell stock
		self.shobj = n_core.init(runobj.msg,runobj.runlimit)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None


		self.dptn  = n_core.fldtp(self.csvin,dtype)

	def __nextCore(self):
	
		line = n_core.getLineDict(self.csvin,self.dptn)
		if line: 
			return line

		n_core.close(self.csvin)
		n_core.cancel(self.shobj)
		raise StopIteration()
	


	def next(self):

		return self.__nextCore()

	def __next__(self):

		return self.__nextCore()

class BlkListIter(object):

	def __init__(self,obj,keys,skeys=None,dtype=None):

		if len(obj.outlist[obj.nowdir])!=0:
			raise Exception("Do not specify last output ")
			return None

		if isinstance(keys,str) :
			newkeys = keys.split(",") 
		elif isinstance(keys,list) :
			newkeys = keys
		else:
			raise Exception("unsuport TYPE")

		dupobj = copy.deepcopy(obj)

		from nysol.mcmd.submod.msortchk import Nysol_Msortchk as msortchk
		sortkeys = copy.deepcopy(newkeys)
		if skeys != None:
			if isinstance(keys,str) :
				sortkeys.extend(skeys.split(","))
			elif isinstance(keys,list) :
				sortkeys.extend(skeys)
			else:
				raise Exception("unsuport TYPE")
			
		runobj = msortchk({"k":sortkeys}).addPre(dupobj)
				
		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)

		# kgshell stock
		self.shobj = n_core.init(runobj.msg,runobj.runlimit)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist,newkeys)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None

		self.dptn  = n_core.fldtp(self.csvin,dtype)

	def __nextCore(self):

		line = n_core.getBlkList(self.csvin,self.dptn)
		if line: 
			return line

		n_core.close(self.csvin)
		n_core.cancel(self.shobj)
		raise StopIteration()
	


	def next(self):
		
		return self.__nextCore()


	def __next__(self):

		return self.__nextCore()

class BlkDictIter(object):

	def __init__(self,obj,keys,skeys=None,dtype=None):

		if len(obj.outlist["o"])!=0:
			raise Exception("Do not specify last output ")
			return None

		if isinstance(keys,str) :
			newkeys = keys.split(",") 
		elif isinstance(keys,list) :
			newkeys = keys
		else:
			raise Exception("unsuport TYPE")

		dupobj = copy.deepcopy(obj)

		from nysol.mcmd.submod.msortchk import Nysol_Msortchk as msortchk
		sortkeys = copy.deepcopy(newkeys)
		if skeys != None:
			if isinstance(keys,str) :
				sortkeys.extend(skeys.split(","))
			elif isinstance(keys,list) :
				sortkeys.extend(skeys)
			else:
				raise Exception("unsuport TYPE")
			
		runobj = msortchk({"k":sortkeys}).addPre(dupobj)

		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)

		# kgshell stock
		self.shobj = n_core.init(runobj.msg,runobj.runlimit)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist,newkeys)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None

		self.dptn  = n_core.fldtp(self.csvin,dtype)

	def __nextCore(self):

		line = n_core.getBlkDict(self.csvin,self.dptn)
		if line: 
			return line

		n_core.close(self.csvin)
		n_core.cancel(self.shobj)
		raise StopIteration()


	def next(self):

		return self.__nextCore()

	def __next__(self):

		return self.__nextCore()


class LineListIterWithInfo(object):

	def __init__(self,obj,keys,skeys=None,dtype=None):

		if len(obj.outlist["o"])!=0:
			raise Exception("Do not specify last output ")
			return None

		
		if isinstance(keys,str) :
			newkeys = keys.split(",") 
		elif isinstance(keys,list) :
			newkeys = keys
		else:
			raise Exception("unsuport TYPE")

		
		dupobj = copy.deepcopy(obj)

		from nysol.mcmd.submod.msortchk import Nysol_Msortchk as msortchk
		sortkeys = copy.deepcopy(newkeys)
		if skeys != None:
			if isinstance(keys,str) :
				sortkeys.extend(skeys.split(","))
			elif isinstance(keys,list) :
				sortkeys.extend(skeys)
			else:
				raise Exception("unsuport TYPE")
			
		runobj = msortchk({"k":sortkeys}).addPre(dupobj)

				
		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)

		# kgshell stock
		self.shobj = n_core.init(runobj.msg,runobj.runlimit)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist,newkeys)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None


		self.dptn  = n_core.fldtp(self.csvin,dtype)
		self.breakPre = True

	def __nextCore(self):
		
		data = n_core.getLineListWithInfo(self.csvin,self.dptn)
		if data: 
			breakTop = self.breakPre
			self.breakPre = data[1]
			return data[0],breakTop,data[1]

		n_core.close(self.csvin)
		n_core.cancel(self.shobj)
		raise StopIteration()
	
	
	def next(self):

		return self.__nextCore()

	def __next__(self):

		return self.__nextCore()
		

class LineDictIterWithInfo(object):

	def __init__(self,obj,keys,skeys=None,dtype=None):

		if len(obj.outlist["o"])!=0:
			raise Exception("Do not specify last output ")
			return None

		dupobj = copy.deepcopy(obj)


		if isinstance(keys,str) :
			newkeys = keys.split(",") 
		elif isinstance(keys,list) :
			newkeys = keys
		else:
			raise Exception("unsuport TYPE")

		from nysol.mcmd.submod.msortchk import Nysol_Msortchk as msortchk

		sortkeys = copy.deepcopy(newkeys)
		if skeys != None:
			if isinstance(keys,str) :
				sortkeys.extend(skeys.split(","))
			elif isinstance(keys,list) :
				sortkeys.extend(skeys)
			else:
				raise Exception("unsuport TYPE")

		runobj = msortchk({"k":sortkeys}).addPre(dupobj)

		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)

		# kgshell stock
		self.shobj = n_core.init(runobj.msg,runobj.runlimit)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist,newkeys)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None

		self.dptn  = n_core.fldtp(self.csvin,dtype)

		self.breakPre = True

	def __nextCore(self):

		data = n_core.getLineDictWithInfo(self.csvin,self.dptn)

		if data: 
			breakTop = self.breakPre
			self.breakPre = data[1]
			return data[0],breakTop,data[1]

		n_core.close(self.csvin)
		n_core.cancel(self.shobj)
		raise StopIteration()

	
	def next(self):

		return self.__nextCore()


	def __next__(self):

		return self.__nextCore()


