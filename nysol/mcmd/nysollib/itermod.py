# -*- coding: utf-8 -*-
import copy
import os
import nysol._nysolshell_core as n_core
# 仮 kgshellへ移行
import psutil as ps

class LineListIter(object):

	def __init__(self,obj,dtype=None,skeys=None,q=False,header=False):

		if len(obj.outlist["o"])!=0:
			raise Exception("Do not specify last output ")
			return None

		if obj.name=="writelist":
			raise Exception("Do not suport writelist ")
			return None

		dupobj = obj.modelcopy()

		from nysol.mcmd.submod.msortchk import Nysol_Msortchk as msortchk
		if skeys != None:
			runobj = msortchk({"k":skeys,"q":q}).addPre(dupobj)
		else :
			runobj = dupobj

		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)

		py_msg=False
		try:
			if get_ipython().__class__.__name__ == 'ZMQInteractiveShell':
				py_msg = True
		except:
			pass
		kgpymsg = os.environ.get('KG_UsingPySysMsg')
		if kgpymsg != None: 
			py_msg = bool(int(kgpymsg))


		# kgshell stock
		self.shobj = n_core.init(runobj.getMsgFlg(),runobj.runlimit,ps.virtual_memory().total,py_msg)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None

		self.header = header
		self.fldname = n_core.fldname(self.csvin)
		self.dptn  = n_core.fldtp(self.csvin,dtype)


	def __nextCore(self):
		if self.header:
			self.header = False
			return self.fldname
		
		line = n_core.getLineList(self.csvin,self.dptn)
		if line: 
			return line

		n_core.close(self.csvin)
		n_core.cancel(self.shobj)
		return
		#raise StopIteration()

	def next(self):

		return self.__nextCore()

	def __next__(self):

		return self.__nextCore()


class LineDictIter(object):

	def __init__(self,obj,dtype=None,skeys=None,q=False):
		

		if len(obj.outlist["o"])!=0:
			raise Exception("Do not specify last output ")
			return None

		if obj.name=="writelist":
			raise Exception("Do not suport writelist ")
			return None

			
		dupobj = obj.modelcopy()

		from nysol.mcmd.submod.msortchk import Nysol_Msortchk as msortchk
		if skeys != None:
			runobj = msortchk({"k":skeys,"q":q}).addPre(dupobj)
		else :
			runobj = dupobj

		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)

		# kgshell stock
		py_msg=False
		try:
			if get_ipython().__class__.__name__ == 'ZMQInteractiveShell':
				py_msg = True
		except:
			pass

		kgpymsg = os.environ.get('KG_UsingPySysMsg')
		if kgpymsg != None: 
			py_msg = bool(int(kgpymsg))

		self.shobj = n_core.init(runobj.getMsgFlg(),runobj.runlimit,ps.virtual_memory().total,py_msg)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None

		self.fldname = n_core.fldname(self.csvin)
		self.dptn  = n_core.fldtp(self.csvin,dtype)

	def __nextCore(self):
	
		line = n_core.getLineDict(self.csvin,self.dptn)
		if line: 
			return line

		n_core.close(self.csvin)
		n_core.cancel(self.shobj)
		return
		#raise StopIteration()

	def next(self):

		return self.__nextCore()

	def __next__(self):

		return self.__nextCore()

class BlkListIter(object):

	def __init__(self,obj,keys,skeys=None,dtype=None,q=False,header=False):

		if len(obj.outlist[obj.nowdir])!=0:
			raise Exception("Do not specify last output ")
			return None

		if obj.name=="writelist":
			raise Exception("Do not suport writelist ")
			return None

		if isinstance(keys,str) :
			newkeys = keys.split(",") 
		elif isinstance(keys,list) :
			newkeys = keys
		else:
			raise Exception("unsuport TYPE")

		dupobj = obj.modelcopy()

		from nysol.mcmd.submod.msortchk import Nysol_Msortchk as msortchk
		sortkeys = copy.deepcopy(newkeys)
		if skeys != None:
			if isinstance(keys,str) :
				sortkeys.extend(skeys.split(","))
			elif isinstance(keys,list) :
				sortkeys.extend(skeys)
			else:
				raise Exception("unsuport TYPE")
			
		runobj = msortchk({"k":sortkeys,"q":q}).addPre(dupobj)
				
		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)

		# kgshell stock
		py_msg=False
		try:
			if get_ipython().__class__.__name__ == 'ZMQInteractiveShell':
				py_msg = True
		except:
			pass

		kgpymsg = os.environ.get('KG_UsingPySysMsg')
		if kgpymsg != None: 
			py_msg = bool(int(kgpymsg))

		self.shobj = n_core.init(runobj.getMsgFlg(),runobj.runlimit,ps.virtual_memory().total,py_msg)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist,newkeys)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None

		self.header = header
		self.fldname = n_core.fldname(self.csvin)
		self.dptn  = n_core.fldtp(self.csvin,dtype)

	def __nextCore(self):

		if self.header:
			self.header = False
			return [self.fldname]

		line = n_core.getBlkList(self.csvin,self.dptn)
		if line: 
			return line

		n_core.close(self.csvin)
		n_core.cancel(self.shobj)
		return
		#raise StopIteration()

	def next(self):
		
		return self.__nextCore()


	def __next__(self):

		return self.__nextCore()

class BlkDictIter(object):

	def __init__(self,obj,keys,skeys=None,dtype=None,q=False):

		if len(obj.outlist["o"])!=0:
			raise Exception("Do not specify last output ")
			return None

		if obj.name=="writelist":
			raise Exception("Do not suport writelist ")
			return None

		if isinstance(keys,str) :
			newkeys = keys.split(",") 
		elif isinstance(keys,list) :
			newkeys = keys
		else:
			raise Exception("unsuport TYPE")

		dupobj = obj.modelcopy()

		from nysol.mcmd.submod.msortchk import Nysol_Msortchk as msortchk
		sortkeys = copy.deepcopy(newkeys)
		if skeys != None:
			if isinstance(keys,str) :
				sortkeys.extend(skeys.split(","))
			elif isinstance(keys,list) :
				sortkeys.extend(skeys)
			else:
				raise Exception("unsuport TYPE")
			
		runobj = msortchk({"k":sortkeys,"q":q}).addPre(dupobj)

		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)

		py_msg=False
		try:
			if get_ipython().__class__.__name__ == 'ZMQInteractiveShell':
				py_msg = True
		except:
			pass

		kgpymsg = os.environ.get('KG_UsingPySysMsg')
		if kgpymsg != None: 
			py_msg = bool(int(kgpymsg))

		# kgshell stock
		self.shobj = n_core.init(runobj.getMsgFlg(),runobj.runlimit,ps.virtual_memory().total,py_msg)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist,newkeys)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None

		self.fldname = n_core.fldname(self.csvin)
		self.dptn  = n_core.fldtp(self.csvin,dtype)

	def __nextCore(self):

		line = n_core.getBlkDict(self.csvin,self.dptn)
		if line: 
			return line

		n_core.close(self.csvin)
		n_core.cancel(self.shobj)
		return
		#raise StopIteration()


	def next(self):

		return self.__nextCore()

	def __next__(self):

		return self.__nextCore()


class LineListIterWithInfo(object):

	def __init__(self,obj,keys,skeys=None,dtype=None,q=False,header=False):

		if len(obj.outlist["o"])!=0:
			raise Exception("Do not specify last output ")
			return None

		if obj.name=="writelist":
			raise Exception("Do not suport writelist ")
			return None

		
		if isinstance(keys,str) :
			newkeys = keys.split(",") 
		elif isinstance(keys,list) :
			newkeys = keys
		else:
			raise Exception("unsuport TYPE")

		
		dupobj = obj.modelcopy()

		from nysol.mcmd.submod.msortchk import Nysol_Msortchk as msortchk
		sortkeys = copy.deepcopy(newkeys)
		if skeys != None:
			if isinstance(keys,str) :
				sortkeys.extend(skeys.split(","))
			elif isinstance(keys,list) :
				sortkeys.extend(skeys)
			else:
				raise Exception("unsuport TYPE")
			
		runobj = msortchk({"k":sortkeys,"q":q}).addPre(dupobj)

				
		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)

		py_msg=False
		try:
			if get_ipython().__class__.__name__ == 'ZMQInteractiveShell':
				py_msg = True
		except:
			pass

		kgpymsg = os.environ.get('KG_UsingPySysMsg')
		if kgpymsg != None: 
			py_msg = bool(int(kgpymsg))

		# kgshell stock
		self.shobj = n_core.init(runobj.getMsgFlg(),runobj.runlimit,ps.virtual_memory().total,py_msg)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist,newkeys)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None

		self.header = header

		self.dptn  = n_core.fldtp(self.csvin,dtype)
		self.fldname = n_core.fldname(self.csvin)
		self.breakPre = True

	def __nextCore(self):

		if self.header:
			self.header = False
			return self.fldname,None,None
		
		data = n_core.getLineListWithInfo(self.csvin,self.dptn)
		if data: 
			breakTop = self.breakPre
			self.breakPre = data[1]
			return data[0],breakTop,data[1]

		n_core.close(self.csvin)
		n_core.cancel(self.shobj)
		return
		#raise StopIteration()
	
	
	def next(self):

		return self.__nextCore()

	def __next__(self):

		return self.__nextCore()
		

class LineDictIterWithInfo(object):

	def __init__(self,obj,keys,skeys=None,dtype=None,q=False):

		if len(obj.outlist["o"])!=0:
			raise Exception("Do not specify last output ")
			return None

		if obj.name=="writelist":
			raise Exception("Do not suport writelist ")
			return None

		dupobj = obj.modelcopy()


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

		runobj = msortchk({"k":sortkeys,"q":q}).addPre(dupobj)

		modlist,iolist,linklist,_ = runobj.makeRunNetwork(True)

		py_msg=False
		try:
			if get_ipython().__class__.__name__ == 'ZMQInteractiveShell':
				py_msg = True
		except:
			pass

		kgpymsg = os.environ.get('KG_UsingPySysMsg')
		if kgpymsg != None: 
			py_msg = bool(int(kgpymsg))

		# kgshell stock
		self.shobj = n_core.init(runobj.getMsgFlg(),runobj.runlimit,ps.virtual_memory().total,py_msg)
		if self.shobj == None:
			raise Exception("can not init shell")
			return None			

		self.csvin = n_core.runiter(self.shobj,modlist,linklist,newkeys)
		if self.csvin == None:
			n_core.cancel(self.shobj)
			raise Exception("can not run iter")
			return None
 
		self.fldname = n_core.fldname(self.csvin)

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
		return
		#raise StopIteration()


	
	def next(self):

		return self.__nextCore()


	def __next__(self):

		return self.__nextCore()


