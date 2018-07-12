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
	# i,o,m,uは別処理(ioは別処理  : f.w. kwdをもとに処理する)

	def __init__(self,name=None,kwd=None,sysadd=False) :

		if kwd == None:
			kwd = {}

		self.name = name
		self.kwd   = kwd
		self.addBySys = sysadd

		self.defaltdir   = "o"
		self.nowdir   = self.defaltdir

		self.inplist ={"i":[],"m":[]}
		self.outlist ={"o":[],"u":[]}
		self.tag = ""

		if "tag" in self.kwd :

			self.tag = self.kwd["tag"]
			del self.kwd["tag"]

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
		

		self.msg=False
		self.runlimit = -1

	def direction(self,dir) :
		self.nowdir = dir
		return self

	def dupdirection(self,dir) :
		self.nowdir = dir
		from nysol.mcmd.submod.mfifo import Nysol_Mfifo as mfifo
		fifoxxx=mfifo(i=self)
		self.outlist[self.nowdir].append(self)
		return fifoxxx


	def _dsp1(self):

		yLimit =40
		pre=[]
		sufmax = int(yLimit/2)
		suf=[ [] for i in range(sufmax) ]
		cnt=0
		sufpos=0

		try:
			xx = itermod.Nysol_MeachIter(self)
			while(True):
				val = next(xx)
				if cnt < yLimit :
					pre.append(val)

				cnt+=1
				suf[sufpos]=val
				sufpos+=1
				if sufpos==sufmax :
					sufpos=0

		except:
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

		outstrList = dspalign.chgDSPstr(pre , cnt > yLimit)

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


	def __iter__(self):

		try:
			x = itermod.Nysol_MeachIter(self)
			while(True):
				yield next(x)

		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)
		
	## generator rap
	def keyblock(self,keys,skeys=None,dtype=None):
		try:
			x = itermod.Nysol_MeachKeyIter(self,keys,skeys,dtype)
			while(True):
				yield next(x)
		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)

	def keyblock_dict(self,keys,skeys=None,dtype=None):
		try:
			x = itermod.Nysol_MeachKeyDictIter(self,keys,skeys,dtype)
			while(True):
				yield next(x)
		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)


	def getline_dict(self,dtype=None):
		try:
			x = itermod.Nysol_MeachDictIter(self,dtype)
			while(True):
				yield next(x)
		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)


	def convtype(self,dtype=None):
		try:
			x = itermod.Nysol_convIter(self,dtype)
			while(True):
				yield next(x)
		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)


	def getline_with_keyflag(self,keys,skeys=None,dtype=None):
		try:
			x = itermod.Nysol_MeachKeyIterWithFlag(self,keys,skeys,dtype)
			while(True):
				yield next(x)
		except GeneratorExit:
			n_core.close(x.csvin)
			n_core.cancel(x.shobj)


	def set_runlimit(self,lim):
		self.runlimit=int(lim)
		return self

	def msgOn(self):
		self.msg=True
		return self

	def msgOff(self):
		self.msg=False
		return self


	def addPre(self,pre): # i以外も可能にする？

		self.inplist["i"].append(pre) 
		pre.outlist[pre.nowdir].append(self)
		return self

	def __ilshift__(self, other):

		pre = other

		while len(pre.inplist["i"])!=0: # ここも注意
			pre = pre.inplist["i"][0]

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
					fifoxxx=mfifo(i=obj.direction(k))
					#obj.outlist[k][0] = fifoxxx
					fifoxxx.outlist["o"]=[outll]

					if len(outll.inplist["i"])!=0 and obj == outll.inplist["i"][0]:
						outll.inplist["i"] = [fifoxxx]
					if len(outll.inplist["m"])!=0 and obj == outll.inplist["m"][0]:
						outll.inplist["m"] = [fifoxxx]

				else:

					outll = obj.outlist[k]
					obj.outlist[k] = []
					teexxx = m2tee(i=obj)
					teexxx.outlist["o"] = [] 

					for outin in outll:
						for ii in range(len(outin.inplist["i"])):
							if obj == outin.inplist["i"][ii]:
								fifoxxx=mfifo(i=teexxx)
								fifoxxx.outlist["o"]=[outin]
								outin.inplist["i"][ii] = fifoxxx

						for ii in range(len(outin.inplist["m"])):
							if obj == outin.inplist["m"][ii]:
								fifoxxx=mfifo(i=teexxx)
								fifoxxx.outlist["o"]=[outin]
								outin.inplist["m"][ii] = fifoxxx


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

							rlmod = mreadlist(xval)
							rlmod.outlist["o"] = [obj]
							obj.inplist[key][i]=rlmod

					if len(obj.inplist[key])>1:
	
						m2cmod  = m2cat(i=obj.inplist[key])
						m2cmod.outlist["o"] = [obj]

						for xval in obj.inplist[key]:
						
							for okey in xval.outlist.keys():
							
								for ii in range(len(xval.outlist[okey])):
									if obj == xval.outlist[okey][ii]:
										xval.outlist[okey][ii] = m2cmod

						obj.inplist[key] = [m2cmod]

				for key in obj.outlist.keys():

					#これOK?複数有ったときは。。どうなる(==1が正しい？。。)
					if len(obj.outlist[key])!=0 and isinstance(obj.outlist[key][0],list) :

						wlmod = mwritelist(obj.outlist[key][0])
						wlmod.inplist["i"]=[obj]
						obj.outlist[key][0] = wlmod

					# 途中OUTPUTチェック
					if len(obj.outlist[key]) > 1:

						for i in range(len(obj.outlist[key])):

							if isinstance(obj.outlist[key][i],str) :

								from nysol.mcmd.submod.writecsv import Nysol_Writecsv as mwritecsv
								wcsv_o = mwritecsv(obj.outlist[key][i])
								wcsv_o.inplist["i"]=[obj]
								obj.outlist[key][i] = wcsv_o

								if obj in dupobj:
									dupobj[obj] += 1
								else:
									dupobj[obj] = 2

								add_mod.append(wcsv_o)	

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

		for obj in self.inplist["i"]:
			if isinstance(obj,NysolMOD_CORE):
				obj.selectUniqMod(sumiobj,modlist)

		for obj in self.inplist["m"]:
			if isinstance(obj,NysolMOD_CORE):
				obj.selectUniqMod(sumiobj,modlist)

	@classmethod
	def makeModList(self,uniqmod,modlist,iolist):

		for obj,no in uniqmod.items():
			modlist[no]= [obj.name,nutil.para2str(obj.kwd),{},obj.tag]
			iolist[no]=[[],[],[],[]]

			for ioobj in obj.inplist["i"]:

				#uniqmodに無ければ今回のルート外のはず
				if isinstance(ioobj,NysolMOD_CORE) and ioobj in uniqmod :
					iolist[no][0].append(uniqmod[ioobj])
				elif isinstance(ioobj,(list,str)):
					modlist[no][2]["i"]=ioobj

			for ioobj in obj.inplist["m"]:
				if isinstance(ioobj,NysolMOD_CORE) and ioobj in uniqmod:
					iolist[no][1].append(uniqmod[ioobj])
				elif isinstance(ioobj,(list,str)):
					modlist[no][2]["m"]=ioobj


			for ioobj in obj.outlist["o"]:
				if isinstance(ioobj,NysolMOD_CORE) and ioobj in uniqmod:
					iolist[no][2].append(uniqmod[ioobj])
				elif isinstance(ioobj,(list,str)):
					modlist[no][2]["o"]=ioobj

			for ioobj in obj.outlist["u"]:
				if isinstance(ioobj,NysolMOD_CORE) and ioobj in uniqmod:
					iolist[no][3].append(uniqmod[ioobj])
				elif isinstance(ioobj,(list,str)):
					modlist[no][2]["u"]=ioobj


	@classmethod
	def getLink(self,iolist,base,to):

		for v in iolist[base][2]:
			if v == to:
				return "o"  

		for v in iolist[base][3]:
			if v == to:
				return "u"
		
		return None
	


	@classmethod
	def makeLinkList(self,iolist,linklist):

		for idx, val in enumerate(iolist):
			rtn = None
			for v in val[0]:
				if isinstance(v, (int)):
					rtn = self.getLink(iolist,v,idx)
					if rtn != None:
						linklist.append([[rtn,v],["i",idx]])

			for v in val[1]:
				if isinstance(v, (int)):
					rtn = self.getLink(iolist,v,idx)
					if rtn != None:
						linklist.append([[rtn,v],["m",idx]])
			

	@classmethod
	def makeRunNetwork(self,mods):

		stocks =[None]*len(mods)
		outfs = [None]*len(mods)

		for i,mod in enumerate(mods):
			if len(mod.outlist["o"]) != 0 :
				stocks[i] = mod.outlist["o"][0]
	
		dupobjs = copy.deepcopy(mods)
		#oが無ければlist出力追加
		runobjs =[None]*len(dupobjs)

		for i, dupobj in enumerate(dupobjs):

			if dupobj.name == "msep" or dupobj.name == "mshuffle" or dupobj.name == "mstdout" or dupobj.name == "runfunc"or dupobj.name == "cmd" : #統一的にする
				runobjs[i]= dupobj			
			elif len(dupobj.outlist["o"])==0:
				if dupobj.name == "writelist":
					dupobj.outlist["o"] =[list()]
					runobjs[i]= dupobj
				else:
					runobjs[i]= dupobj.writelist(list())

			elif dupobj.name != "writelist" and isinstance(dupobj.outlist["o"][0],list): 
				runobj = dupobj.writelist(stocks[i])
				dupobj.outlist["o"] = [runobj]
				runobjs[i]= runobj

			elif isinstance(dupobj.outlist["o"][0],list):				
				dupobj.outlist["o"][0] = stocks[i]
				runobjs[i]= dupobj
			else:
				runobjs[i]= dupobj
	
			if dupobj.name == "msep" or dupobj.name == "mshuffle"  or dupobj.name == "mstdout" or dupobj.name == "runfunc" or dupobj.name == "cmd" : #統一的にする
				outfs[i] = []
			else:
				outfs[i] = runobjs[i].outlist["o"][0]

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

		modlist,iolist,linklist,outfs = NysolMOD_CORE.makeRunNetwork(mods)

		shobj = n_core.init(msgF,modlimt)

		n_core.runLx(shobj,modlist,linklist)

		return outfs


		
	@classmethod
	def drawModels(self,mod,fname=None):
		modlist,iolist,linklist,_ = NysolMOD_CORE.makeRunNetwork(mod)
		ndraw.chageSVG(modlist,iolist,linklist,fname)		

	@classmethod
	def drawModelsD3(self,mod,fname=None):

		modlist,iolist,linklist,x = NysolMOD_CORE.makeRunNetwork(mod)
		ndraw.chageSVG_D3(modlist,iolist,linklist,fname)



	#GRAPH表示 #deepコピーしてからチェック
	@classmethod
	def modelInfos(self,mod):

		dupshowobjs = copy.deepcopy(mod)
		showobjs =[]
		rtnlist = []
		# 最終形式チェック
		for dupshowobj in dupshowobjs:
			if dupshowobj.name == "msep" or dupshowobj.name == "mshuffle" or dupshowobj.name == "mstdout" or dupshowobj.name == "cmd":
				showobjs.append(dupshowobj)
			elif len(dupshowobj.outlist["o"])==0:
				showobjs.append(dupshowobj.writelist(rtnlist))
			elif dupshowobj.name != "writelist" and isinstance(dupshowobj.outlist["o"][0],list): 
				showobj = dupshowobj.writelist(dupshowobj.outlist["o"][0])
				dupshowobj.outlist["o"] = [showobj]
				showobjs.append(showobj)
			else:
				showobjs.append(dupshowobj)

		self.change_modNetworks(showobjs)

		uniqmod={} 
		sumiobj= set([])

		for mod in showobjs:
			mod.selectUniqMod(sumiobj,uniqmod)


		modlist=[None]*len(uniqmod) #[[name,para]]
		iolist=[None]*len(uniqmod) #[[iNo],[mNo],[oNo],[uNo]]
		self.makeModList(uniqmod,modlist,iolist)

		linklist=[]
		self.makeLinkList(iolist,linklist)
		
		return {"modlist":modlist,"iolist":iolist,"linklist":linklist}


	def run(self,**kw_args):

		return NysolMOD_CORE.runs([self],**kw_args)[0]


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


