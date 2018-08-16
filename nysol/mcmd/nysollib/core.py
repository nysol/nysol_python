# -*- coding: utf-8 -*-
import shlex
import re
import copy
import os, sys

import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib import nysolutil as nutil
from nysol.mcmd.nysollib import draw as ndraw
from nysol.mcmd.nysollib import dspalign
from nysol.mcmd.nysollib import itermod


class NysolMOD_CORE(object):

	def __init__(self,name=None,kwd=None) :

		self.name     = name
		self.addBySys = False
		self.msg      = False
		self.runlimit = -1
		self.kwd      = {}
		self.inplist  = {}
		self.outlist  = {}
		self.stdodir  = None
		self.stdidir  = None

		self.disabled_ouputlist = False
		self.modWeight = 0

		
		if hasattr(self,'_inkwd') and len(self._inkwd)>0 :
			self.modWeight += len(self._inkwd) * 4
			self.stdidir = self._inkwd[0].rstrip("=")
			for k in self._inkwd:
				self.inplist[k.rstrip("=")] = []

		if hasattr(self,'_outkwd')  and len(self._outkwd)>0 :

			self.modWeight += len(self._outkwd) * 1

			self.stdodir = self._outkwd[0].rstrip("=")
			for k in self._outkwd:
				self.outlist[k.rstrip("=")] = []
				
		else:

			self.disabled_ouputlist = True
		
		if hasattr(self,'_disabled_ouputlist'):
			self.disabled_ouputlist = self._disabled_ouputlist

		if self.modWeight == 0:
			self.modWeight += 1

		if kwd != None:
			self.kwd   = kwd

		self.tag = ""
		self.nowdir   = self.stdodir

		if "tag" in self.kwd :

			self.tag = self.kwd["tag"]
			del self.kwd["tag"]

		if "sysadd" in self.kwd :

			self.addBySys = self.kwd["sysadd"]
			del self.kwd["sysadd"]

		for key in self.inplist.keys():

			if key in self.kwd :

				if isinstance(self.kwd[key],NysolMOD_CORE):	

					self.kwd[key].outlist[self.kwd[key].nowdir].append(self)
					self.inplist[key].append(self.kwd[key])
				
				elif isinstance(self.kwd[key],list):

					if isinstance(self.kwd[key][0],list):	

						self.inplist[key].append(self.kwd[key])
		
					else:

						for kval in self.kwd[key]:

							if isinstance(kval,NysolMOD_CORE):	
								kval.outlist[kval.nowdir].append(self)
	
							self.inplist[key].append(kval)

				else:

					self.inplist[key].append(self.kwd[key])
		
				del self.kwd[key]
		

		for key in self.outlist.keys():
	
			if key in self.kwd :

				self.outlist[key].append(self.kwd[key])
				del self.kwd[key]


	def redirect(self,dir) :
		self.nowdir = dir
		return self

	def dupdirection(self,dir) :
		self.nowdir = dir
		from nysol.mcmd.submod.mfifo import Nysol_Mfifo as mfifo
		fifoxxx=mfifo(i=self,sysadd=True)
		self.outlist[self.nowdir].append(self)
		return fifoxxx

	def okwdObjCnt(self):
		count=0
		for key in self.outlist.keys():
			count += len(self.outlist[key])	
		return count


	def _dspselct(self,ylim,xlim=20):

		yLimit = ylim
		pre=[]
		head=None
		sufmax = int(yLimit/2)
		xmid = int(xlim/2)

		suf=[ [] for i in range(sufmax) ]
		cnt=0
		sufpos=0

		dupobj = copy.deepcopy(self)

		# 不要 mod 除去 
		for k in dupobj.outlist.keys():
			dupobj.outlist[k].clear()


		try:
			xx = itermod.LineListIter(dupobj)
			if len(xx.fldname)>xlim:
				head = xx.fldname[:xmid]+["..."] + xx.fldname[-xmid:]
			else:
				head = xx.fldname 

			while(True):
				val = next(xx)
				xval = val
				if cnt < yLimit :
					if len(val)>xlim:
						xval = val[:xmid]+["..."] + val[-xmid:]

					pre.append(xval)

				cnt+=1
				suf[sufpos]=xval
				sufpos+=1
				if sufpos==sufmax :
					sufpos=0

		except StopIteration:
			pass

		if(cnt > yLimit): 
			ppos = sufmax
			spos = sufpos
			for _ in range(sufmax):

				pre[ppos] = suf[spos]
				ppos += 1
				spos += 1
				if spos==sufmax :
					spos=0

		return pre,cnt,head


	def _dsp1(self):

		yLimit =40

		dspdata ,cnt ,head = self._dspselct(yLimit)

		outstrList = dspalign.chgDSPstr(dspdata , cnt > yLimit)

		return "\n".join(outstrList)



	def _dsp2(self):

		yLimit =40

		dspdata ,cnt ,head = self._dspselct(yLimit)

		outstrList = dspalign.chgDSPhtml(dspdata , yLimit , cnt, head)

		return "\n".join(outstrList)



	def __str__(self):
		import os

		dsptp = os.getenv("NYSOL_MOD_DSP_TYPE", "0")


		if dsptp == "1":
		
			return self._dsp1()

		else:

			return "<{} at {}>".format(self.__class__.__name__,hex(id(self)))


	def __repr__(self):

		return str(self)

	def _repr_html_(self):
		return self._dsp2()

	def __iter__(self):

		try:
			x = itermod.LineListIter(self)
			while(True):
				yield next(x)

		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)

	# __getLineList処理は同じ
	def convtype(self,dtype=None):
		try:
			x = itermod.LineListIter(self,dtype)
			while(True):
				yield next(x)
		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)

	def __getLineList(self,dtype=None,skey=None):

		try:
			x = itermod.LineListIter(self,dtype,skey)
			while(True):
				yield next(x)

		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)

	def __getLineListWithInfo(self,keys,skeys=None,dtype=None):
		try:
			x = itermod.LineListIterWithInfo(self,keys,skeys,dtype)
			while(True):
				yield next(x)
		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)


	def __getLineDictWithInfo(self,keys,skeys=None,dtype=None):

		try:
			x = itermod.LineDictIterWithInfo(self,keys,skeys,dtype)
			while(True):
				yield next(x)

		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)

	def __getLineDict(self,dtype=None,skey=None):

		try:

			x = itermod.LineDictIter(self,dtype,skey)
			while(True):
				yield next(x)

		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)




	# return generator
	def getline(self,dtype=None,keys=None,skeys=None,otype="list"):

		if otype == "list":

			if keys!=None:

				return self.__getLineListWithInfo(keys,skeys,dtype)

			elif skeys!=None:


				return self.__getLineList(dtype,skeys)

			else:
				return self.__getLineList(dtype,None)


		elif otype == "dict":

			if keys!=None :

				return self.__getLineDictWithInfo(keys,skeys,dtype)

			elif skeys!=None:

				return self.__getLineDict(dtype,skeys)

			else:
				
				return self.__getLineDict(dtype,None)

		else:

			raise Exception("unsuport rtype" + rtype)




	## generator rap
	def __getBlockList(self,keys,skeys=None,dtype=None):

		try:

			x = itermod.BlkListIter(self,keys,skeys,dtype)
			while(True):
				yield next(x)

		except GeneratorExit:

			n_core.close(x.csvin)
			n_core.cancel(x.shobj)

	def __getBlockDict(self,keys,skeys=None,dtype=None):

		try:

			x = itermod.BlkDictIter(self,keys,skeys,dtype)
			while(True):
				yield next(x)

		except GeneratorExit:

			n_core.close(x.csvin)
			n_core.cancel(x.shobj)

	# return generator
	def keyblock(self,keys,skeys=None,dtype=None,otype="list"):

		if otype == "list":

			return self.__getBlockList(keys,skeys,dtype)

		elif otype == "dict":

			return self.__getBlockDict(keys,skeys,dtype)

		else : 

			raise Exception("unsuport rtype" + rtype)


	def set_runlimit(self,lim):
		self.runlimit=int(lim)
		return self

	def msgOn(self):
		self.msg=True
		return self

	def msgOff(self):
		self.msg=False
		return self


	def addPre(self,pre): 

		self.inplist[self.stdidir].append(pre) 
		pre.outlist[pre.nowdir].append(self)
		return self

	def __ilshift__(self, other):

		pre = other
		precnt = len(pre.inplist[pre.stdidir])
		while precnt != 0:
			if precnt > 1 :
				raise Exception("Multiple input")
			pre = pre.inplist[pre.stdidir][0]
			precnt = len(pre.inplist[pre.stdidir])

		pre.addPre(self)
		return other

	def __rlshift__(self, other):

		if(other!=None):
			raise Exception("unsuport TYPE")

		return self

	def paraUpdate(self,kw):
		# f.w キーワードチェック入れる

		self.kwd.update(kw)

		for key in self.inplist.keys():

			if key in self.kwd :

				self.inplist[key].append(self.kwd[key])
				del self.kwd[key]


		for key in self.outlist.keys():

			if key in self.kwd :

				self.outlist[key].append(self.kwd[key])
				del self.kwd[key]

	@classmethod
	def check_dupObjSub(self,sumiobj,dupobj,obj):

		if obj in sumiobj:
			if obj in dupobj:
				dupobj[obj] += 1
			else:
				dupobj[obj] = 2
			return True 
		else:
			if not isinstance(obj,str):
				sumiobj.add(obj)
			return False
	
	def check_dupObj(self,sumiobj,dupobj):
		#o=でチェックできるはずなので　いらなくなるはず 		
		if NysolMOD_CORE.check_dupObjSub(sumiobj,dupobj,self) ==True :
			return

		for key in self.inplist.keys():

			if len(self.inplist[key]) != 0 :

				for xval in self.inplist[key]:

					if isinstance(xval,NysolMOD_CORE):
						xval.check_dupObj(sumiobj,dupobj)

					elif isinstance(xval,str):
						self.check_dupObjSub(sumiobj,dupobj,xval)

					elif isinstance(xval,list):
						pass

		return			
 
	@classmethod
	def addTee(self,dupobj): 
		from nysol.mcmd.submod.m2tee import Nysol_M2tee as m2tee
		from nysol.mcmd.submod.mfifo import Nysol_Mfifo as mfifo

		for obj in dupobj:

			for k in obj.outlist.keys():

				if len(obj.outlist[k])==0:
					continue
				elif len(obj.outlist[k])==1: #fifoのみ追加(o,u対策)

					if isinstance(obj.outlist[k][0],str):
						continue 

					outll = obj.outlist[k][0]
					obj.outlist[k] = []
					fifoxxx=mfifo(i=obj.direction(k),sysadd=True)
					fifoxxx.outlist[fifoxxx.nowdir]=[outll]

					for ki in outll.inplist: # 0だけOK?
						if len(outll.inplist[ki])!=0 and obj == outll.inplist[ki][0]:						
							outll.inplist[ki] = [fifoxxx]

				else:

					outll = obj.outlist[k]
					obj.outlist[k] = []
					teexxx = m2tee(i=obj,sysadd=True)
					teexxx.outlist[teexxx.nowdir] = [] 

					for outin in outll:
						for ki in outin.inplist: # 0だけOK?

							for ii in range(len(outin.inplist[ki])):
								if obj == outin.inplist[ki][ii]:
									fifoxxx=mfifo(i=teexxx,sysadd=True)
									fifoxxx.outlist[fifoxxx.nowdir]=[outin] 
									outin.inplist[ki][ii] = fifoxxx



		# no buffer Version
		#for obj in dupobj:
		#	outll = obj.outlist["o"]
		#	teexxx = m2tee(i=obj)
		#	obj.outlist["o"]= [teexxx]
		#	teexxx.outlist["o"] = outll
		#	for outin in outll:
		#		if len(outin.inplist["i"])!=0 and obj == outin.inplist["i"][0]:
		#			outin.inplist["i"] = [teexxx]
		#		if len(outin.inplist["m"])!=0 and obj == outin.inplist["m"][0]:
		#			outin.inplist["m"] = [teexxx]




	@classmethod
	def addIoMod(self,sumiobj,dupobj):
		# add list read
		from nysol.mcmd.submod.readlist import Nysol_Readlist as mreadlist
		from nysol.mcmd.submod.writelist import Nysol_Writelist as mwritelist
		from nysol.mcmd.submod.m2cat import Nysol_M2cat as m2cat
		add_mod =[]

		for obj in sumiobj:
			if isinstance(obj,NysolMOD_CORE):
				if obj.name=="readlist":
					continue
				if obj.name=="writelist":
					continue
				if obj.name=="m2cat":
					continue

				for key in obj.inplist.keys():

					for i,xval in enumerate(obj.inplist[key]):

						if isinstance(xval,list) :

							rlmod = mreadlist(xval,sysadd=True)
							rlmod.outlist[rlmod.nowdir] = [obj]
							obj.inplist[key][i]=rlmod

					if len(obj.inplist[key])>1:
	
						m2cmod  = m2cat(i=obj.inplist[key],sysadd=True)
						m2cmod.outlist[m2cmod.nowdir] = [obj]

						for xval in obj.inplist[key]:
						
							for okey in xval.outlist.keys():
							
								for ii in range(len(xval.outlist[okey])):
									if obj == xval.outlist[okey][ii]:
										xval.outlist[okey][ii] = m2cmod

						obj.inplist[key] = [m2cmod]

				for key in obj.outlist.keys():

					# 途中OUTPUTチェック
					if len(obj.outlist[key]) > 1:

						for i in range(len(obj.outlist[key])):

							if isinstance(obj.outlist[key][i],str) :

								from nysol.mcmd.submod.writecsv import Nysol_Writecsv as mwritecsv
								wcsv_o = mwritecsv(obj.outlist[key][i],sysadd=True)
								wcsv_o.inplist[wcsv_o.stdidir]=[obj]
								obj.outlist[key][i] = wcsv_o
								if obj in dupobj:
									dupobj[obj] += 1
								else:
									dupobj[obj] = 2
								add_mod.append(wcsv_o)

							if isinstance(obj.outlist[key][i],list) :
								wlmod = mwritelist(obj.outlist[key][i],sysadd=True)
								wlmod.inplist[wlmod.stdidir]=[obj]
								obj.outlist[key][i] = wlmod
								if obj in dupobj:
									dupobj[obj] += 1
								else:
									dupobj[obj] = 2
								add_mod.append(wlmod)	

		return add_mod		
	
	@classmethod
	def change_modNetworks(self,mods):
		sumiobj=set([])
		dupobj={}
		for mod in mods:
			mod.check_dupObj(sumiobj,dupobj)

		add_Dmod = NysolMOD_CORE.addIoMod(sumiobj,dupobj)

		if len(dupobj)!=0:
			NysolMOD_CORE.addTee(dupobj)
			
		mods.extend(add_Dmod)


	def change_modNetwork(self):

		NysolMOD_CORE.change_modNetworks([self])

	def selectUniqMod(self,sumiobj,modlist):
		if self in sumiobj:
			return None

		pos = len(sumiobj)
		sumiobj.add(self)
		modlist[self]=pos
		
		for k in self.inplist.keys():
			for obj in self.inplist[k]:
				if isinstance(obj,NysolMOD_CORE):
					obj.selectUniqMod(sumiobj,modlist)


	@classmethod
	def makeModList(self,uniqmod,modlist,iolist): # iolist標準化

		for obj,no in uniqmod.items():
			# name,para,ipara,opara,tag
			modlist[no]= [obj.name,nutil.para2str(obj.kwd),{},{},obj.tag,obj.addBySys,obj.modWeight]
			iolist[no]=[[],[]]

			for k in obj.inplist.keys():

				for ioobj in obj.inplist[k]:
					#uniqmodに無ければ今回のルート外のはず
					if isinstance(ioobj,NysolMOD_CORE) and ioobj in uniqmod :
						iolist[no][0].append([uniqmod[ioobj],k])
					elif isinstance(ioobj,(list,str)):
						modlist[no][2][k]=ioobj

			for k in obj.outlist.keys():

				for ioobj in obj.outlist[k]:
					#uniqmodに無ければ今回のルート外のはず
					if isinstance(ioobj,NysolMOD_CORE) and ioobj in uniqmod :
						iolist[no][1].append([uniqmod[ioobj],k])
					elif isinstance(ioobj,(list,str)):
						modlist[no][3][k]=ioobj


	@classmethod
	def getLink(self,iolist,base,to):

		for v,dirct in iolist[base][1]:
			if v == to:
				return dirct
		
		return None
	


	@classmethod
	def makeLinkList(self,iolist,linklist):

		for idx, val in enumerate(iolist):
			rtn = None
			for v,direct in val[0]:
				if isinstance(v, (int)):
					rtn = self.getLink(iolist,v,idx)
					if rtn != None:
						linklist.append([[rtn,v],[direct,idx]])
			

	
	@classmethod
	def graphFindList(self,mod,sumiobj,liststk):

		if mod in sumiobj:
			return
		
		sumiobj.add(mod)

		for k in mod._outkwd:
			for oobj in mod.outlist[k]:
				if isinstance(oobj,list):
					liststk.append(oobj)
			
		for key in mod.inplist.keys():
			for iobj in mod.inplist[key]:
				if isinstance(iobj,NysolMOD_CORE):
					self.graphFindList(iobj,sumiobj,liststk)
	
		return


	@classmethod
	def graphSetList(self,mod,sumiobj,liststk,pos):

		if mod in sumiobj:
			return pos
		
		sumiobj.add(mod)

		for k in mod._outkwd:
			for ipos in range(len(mod.outlist[k])):
				if isinstance(mod.outlist[k][ipos],list):
					mod.outlist[k][ipos] = liststk[pos]
					pos+=1
			
		for key in mod.inplist.keys():
			for iobj in mod.inplist[key]:
				if isinstance(iobj,NysolMOD_CORE):
					pos = self.graphSetList(iobj,sumiobj,liststk,pos)
	
		return pos

	@classmethod
	def makeRunNetworks(self,mods,useIter=False):

		# stock list obj before deepcopy
		listStks =[]
		sumiobj=set([])

		for mod in mods: 
			self.graphFindList(mod,sumiobj,listStks)

		dupobjs = copy.deepcopy(mods)

		# set list obj after deepcopy		
		sumiobj=set([])

		for mod in dupobjs: 
			self.graphSetList(mod,sumiobj,listStks,0)

		outfs = [None]*len(mods)		

		runobjs =[None]*len(dupobjs)

		for i, dupobj in enumerate(dupobjs):

			# 不要 mod 除去 & 元 output object セット
			for k in dupobj.outlist.keys():
				newlist = [e for e in dupobj.outlist[k] if not isinstance(e,NysolMOD_CORE)]
				dupobj.outlist[k].clear()
				dupobj.outlist[k].extend(newlist)

			if True == dupobj.disabled_ouputlist or useIter: #統一的にする
				#  最終list不可はそのまま
				runobjs[i]= dupobj			
				outfs[i] = []
			elif dupobj.name == "writelist":

				if dupobj.okwdObjCnt() == 0:
	
					dupobj.outlist[dupobj.nowdir] =[list()]
						
				runobjs[i]= dupobj

				outfs[i] = runobjs[i].outlist[runobjs[i].nowdir][0]

			else:

				if dupobj.okwdObjCnt() == 0:

					runobjs[i]= dupobj.writelist(list(),sysadd=True)
				
				else:			
					#こここれでいい？
					runobj = dupobj

					for k in dupobj.outlist.keys():

						for ki,oobj in enumerate(dupobj.outlist[k]):

							if isinstance(oobj,list):
								runobj = dupobj.writelist(dupobj.outlist[k][ki],sysadd=True)
								dupobj.outlist[k][ki] = runobj
								break

					runobjs[i]= runobj

				outfs[i] = runobjs[i].outlist[runobjs[i].nowdir][0]
			

		self.change_modNetworks(runobjs)
		
		uniqmod={} 
		sumiobj= set([])


		for mod in runobjs:
			mod.selectUniqMod(sumiobj,uniqmod)

		modlist=[None]*len(uniqmod) #[[name,para]]
		iolist=[None]*len(uniqmod) #[[iNo],[mNo],[oNo],[uNo]]

		self.makeModList(uniqmod,modlist,iolist)
		linklist=[]
		self.makeLinkList(iolist,linklist)
		return modlist,iolist,linklist,outfs


	@classmethod
	def runs(self,mods,**kw_args):

		msgF =mods[0].msg
		modlimt = mods[0].runlimit

		if "msg" in kw_args:
			if kw_args["msg"] == "on" :
				msgF = True

		if "runlimit" in kw_args:
			modlimt = int(kw_args["runlimit"])

		modlist,iolist,linklist,outfs = NysolMOD_CORE.makeRunNetworks(mods)
		
		# 仮 kgshellへ移行
		import psutil as ps
		shobj = n_core.init(msgF,modlimt,ps.virtual_memory().total)


		n_core.runLx(shobj,modlist,linklist)

		return outfs


		
	@classmethod
	def drawModels(self,mod,fname=None):

		modlist,iolist,linklist,_ = NysolMOD_CORE.makeRunNetworks(mod)
		ndraw.chageSVG(modlist,iolist,linklist,fname)		

	@classmethod
	def drawModelsD3(self,mod,fname=None):

		modlist,iolist,linklist,x = NysolMOD_CORE.makeRunNetworks(mod)
		ndraw.chageSVG_D3(modlist,iolist,linklist,fname)



	#GRAPH表示 #deepコピーしてからチェック
	@classmethod
	def modelInfos(self,mod):

		modlist,iolist,linklist,_ = NysolMOD_CORE.makeRunNetworks(mod)
		return {"modlist":modlist,"iolist":iolist,"linklist":linklist}


	def run(self,**kw_args):

		return NysolMOD_CORE.runs([self],**kw_args)[0]


	def makeRunNetwork(self,useIter=False):

		return NysolMOD_CORE.makeRunNetworks([self],useIter)


	def drawModel(self,fname=None):

		NysolMOD_CORE.drawModels([self],fname)


	def drawModelD3(self,fname=None):

		NysolMOD_CORE.drawModelsD3([self],fname)


	def modelInfo(self):

		return NysolMOD_CORE.modelInfos([self])


def runs(val,**kw_args):
	return NysolMOD_CORE.runs(val,**kw_args)

def drawModels(val,fname):
	return NysolMOD_CORE.drawModels(val,fname)

def drawModelsD3(val,fname):
	return NysolMOD_CORE.drawModelsD3(val,fname)

def modelInfos(val):
	return NysolMOD_CORE.modelInfos(val)


