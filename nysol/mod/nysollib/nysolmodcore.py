# -*- coding: utf-8 -*-
import shlex
import re
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib import nysolutil as nutil
from nysol.mod.nysollib import draw as ndraw

from multiprocessing import Pool
import copy
import os, sys


#def mulit_run(val):
#	cc = n_core.init(val[2])
#	return n_core.runL(cc,val[0],val[1])


def runs(val,**kw_args):
	return NysolMOD_CORE.runs(val,**kw_args)

def drawModels(val,fname):
	return NysolMOD_CORE.drawModels(val,fname)

def drawModelsD3(val,fname):
	return NysolMOD_CORE.drawModelsD3(val,fname)


def modelInfos(val):
	return NysolMOD_CORE.modelInfos(val)

class NysolMOD_CORE(object):
	# i,o,m,uは別処理(ioは別処理  : f.w. kwdをもとに処理する)
	def __init__(self,name=None,kwd=None) :
		if kwd == None:
			kwd = {}
		self.name = name
		self.kwd   = kwd
		self.defaltdir   = "o"
		self.nowdir   = self.defaltdir

		self.inplist ={"i":[],"m":[]}
		self.outlist ={"o":[],"u":[]}
		self.tag = ""
		self.dlog = ""

		if "tag" in self.kwd :
			self.tag = self.kwd["tag"]
			del self.kwd["tag"]

		if "dlog" in self.kwd :
			self.dlog = self.kwd["dlog"]
			del self.kwd["dlog"]


		if "i" in self.kwd :
			if isinstance(self.kwd["i"],NysolMOD_CORE):	
				self.kwd["i"].outlist[self.kwd["i"].nowdir].append(self)
			self.inplist["i"].append(self.kwd["i"])
			del self.kwd["i"]

		if "o" in self.kwd :
			self.outlist["o"].append(self.kwd["o"])
			del self.kwd["o"]
		
		if "m" in self.kwd :
			if isinstance(self.kwd["m"],NysolMOD_CORE):	
				self.kwd["m"].outlist[self.kwd["m"].nowdir].append(self)

			self.inplist["m"].append(self.kwd["m"])
			
			del self.kwd["m"]

		if "u" in self.kwd :
			self.outlist["u"].append(self.kwd["u"])
			del self.kwd["u"]

		self.msg=False


	def direction(self,dir) :
		self.nowdir = dir
		return self


	def __iter__(self):
		return Nysol_MeachIter(self)

	def keyblock(self,keys,skeys):
		x = Nysol_MeachKeyIter(self,keys,skeys)
		while(True):
			yield next(x)

	def msgOn(self):
		self.msg=True
		return self

	def msgOff(self):
		self.msg=False
		return self


	def addPre(self,pre):
		self.inplist["i"].append(pre) 
		pre.outlist[pre.nowdir].append(self)
		#if len(self.inplist["m"]) != 0 and isinstance(self.inplist["m"][0],NysolMOD_CORE):			
		#	self.inplist["m"][0].outlist[self.inplist["m"][0].nowdir].append(self)
		return self

	def __ilshift__(self, other):
		pre = other
		while len(pre.inplist["i"])!=0:
			pre = pre.inplist["i"][0]

		pre.addPre(self)
		return other

	def __rlshift__(self, other):
		if(other!=None):
			raise Exception("unsuport TYPE")
		return self
		
		
	# PRIVATEにする？
	def para2str(self):
		rtnStr = ""
		for k, v in self.kwd.items():
			if isinstance(v,bool) :
				if v==True:
					rtnStr += "-" + k + " "
			elif isinstance(v,str) :
				rtnStr += k + "=" + v + " "
			elif isinstance(v,float) :
				rtnStr += k + "=" + str(v) + " "
			elif isinstance(v,int) :
				rtnStr += k + "=" + str(v) + " "
			elif isinstance(v,list) :
				plist = []
				for val in v:
					if isinstance(val,str) :
						plist.append(val)
					elif isinstance(val,float) or isinstance(v,int) :
						plist.append(str(val))
				rtnStr += k + "=" + ",".join(plist) + " "
				
		return rtnStr
	
	# f.w キーワードチェック入れる
	def paraUpdate(self,kw):
		self.kwd.update(kw)

		if "i" in self.kwd :
			self.inplist["i"].append(self.kwd["i"])
			del self.kwd["i"]

		if "o" in self.kwd :
			self.outlist["o"].append(self.kwd["o"])
			del self.kwd["o"]
		
		if "m" in self.kwd :
			self.inplist["m"].append(self.kwd["m"])
			del self.kwd["m"]

		if "u" in self.kwd :
			self.outlist["u"].append(self.kwd["u"])
			del self.kwd["u"]

	
	@classmethod
	def check_dupObjSub(self,sumiobj,dupobj,obj):
		#if not isinstance(obj,str):
		#	print("------mod---------")
		#	print(obj)
		#	print(obj.name)
		#	print(obj.para2str())
		#	print("i=",obj.inplist["i"])
		#	print("m=",obj.inplist["m"])
		#	print("o=",obj.outlist["o"])
		#	print("u=",obj.outlist["u"])
		#	print("------mod---------")			
		#else:
		#	print("------mod---------")			
		#	print(obj)
		#	print("------mod---------")			

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

		if len(self.inplist["i"]) != 0 :
			if isinstance(self.inplist["i"][0],NysolMOD_CORE):
				self.inplist["i"][0].check_dupObj(sumiobj,dupobj)

			elif isinstance(self.inplist["i"][0],str):
				self.check_dupObjSub(sumiobj,dupobj,self.inplist["i"][0])

			elif isinstance(self.inplist["i"][0],list):
				pass


		if len(self.inplist["m"]) != 0 :
			if isinstance(self.inplist["m"][0],NysolMOD_CORE):
				self.inplist["m"][0].check_dupObj(sumiobj,dupobj) 
			elif isinstance(self.inplist["m"][0],str):
				self.check_dupObjSub(sumiobj,dupobj,self.inplist["m"][0])

			elif isinstance(self.inplist["m"][0],list):
				pass


		return			
 
	@classmethod
	def addTee(self,dupobj): 
		from nysol.mod.submod.m2tee import Nysol_M2tee as m2tee
		from nysol.mod.submod.mfifo import Nysol_Mfifo as mfifo
		for obj in dupobj:
			#print("----add tee----") 
			#print(obj)
			#print(obj.name)
			#print(obj.para2str())
			#print("i=",obj.inplist["i"])
			#print("m=",obj.inplist["m"])
			#print("o=",obj.outlist["o"])
			#print("u=",obj.outlist["u"])
			#print("----add tee----") 
			for k in obj.outlist.keys():
				#print(k,obj.outlist[k])
				if len(obj.outlist[k])==0:
					continue
				elif len(obj.outlist[k])==1: #fifoのみ追加
					outll = obj.outlist[k][0]
					fifoxxx=mfifo(i=obj)
					obj.outlist[k][0] = fifoxxx
					fifoxxx.outlist["o"]=[outll]
					if len(outll.inplist["i"])!=0 and obj == outll.inplist["i"][0]:
						outll.inplist["i"] = [fifoxxx]
					if len(outll.inplist["m"])!=0 and obj == outll.inplist["m"][0]:
						outll.inplist["m"] = [fifoxxx]

				else:
					outll = obj.outlist[k]
					obj.outlist[k] = []
					#print("aaaaaa1")
					#print(outll)
					#print("aaaaaa2")
					teexxx = m2tee(i=obj)
					teexxx.outlist["o"] = [] 
					obj.outlist[k].append(teexxx)
					#print(outll)

					"""これだとだめ
					print(obj.outlist)
					outll = obj.outlist[k]
					print("aaaaaa1")
					print(outll)
					print("aaaaaa2")
					teexxx = m2tee(i=obj)
					teexxx.outlist["o"] = [] 
					obj.outlist[k] = [teexxx]
					print(outll)
					print("aaaaaa3")
					"""

					for outin in outll:
						if len(outin.inplist["i"])!=0 and obj == outin.inplist["i"][0]:
							fifoxxx=mfifo(i=teexxx)
							teexxx.outlist["o"].append(fifoxxx)
							fifoxxx.outlist["o"]=[outin]
							outin.inplist["i"] = [fifoxxx]
						if len(outin.inplist["m"])!=0 and obj == outin.inplist["m"][0]:
							fifoxxx=mfifo(i=teexxx)
							teexxx.outlist["o"].append(fifoxxx)
							fifoxxx.outlist["o"]=[outin]
							outin.inplist["m"] = [fifoxxx]

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
		from nysol.mod.submod.readlist import Nysol_Readlist as mreadlist
		from nysol.mod.submod.writelist import Nysol_Writelist as mwritelist
		add_mod =[]
		for obj in sumiobj:
			if isinstance(obj,NysolMOD_CORE):
				if obj.name=="readlist":
					continue
				if obj.name=="writelist":
					continue
				if len(obj.inplist["i"])!=0 and isinstance(obj.inplist["i"][0],list) :
					rlmod = mreadlist(obj.inplist["i"][0])
					rlmod.outlist["o"] = [obj]
					obj.inplist["i"][0]=rlmod

				if len(obj.inplist["m"])!=0 and isinstance(obj.inplist["m"][0],list) :
					rlmod = mreadlist(inplist["m"][0])
					rlmod.outlist["o"] = [obj]
					obj.inplist["m"][0]=rlmod
			
				if len(obj.outlist["o"])!=0 and isinstance(obj.outlist["o"][0],list) :
					wlmod = mwritelist(obj.outlist["o"][0])
					wlmod.inplist["i"]=[obj]
					obj.outlist["o"][0] = wlmod


				if len(obj.outlist["u"])!=0 and isinstance(obj.outlist["u"][0],list) :
					wlmod = mwritelist(obj.outlist["u"][0])
					wlmod.inplist["i"]=[obj]
					obj.outlist["u"][0] = wlmod
					
				if obj.dlog != "" :
					if len(obj.outlist["o"])!=0:
						from nysol.mod.submod.writecsv import Nysol_Writecsv as mwritecsv
						wcsv_o = mwritecsv(obj.dlog+"_o")
						wcsv_o.inplist["i"]=[obj]
						obj.outlist["o"].append(wcsv_o)
						if obj in dupobj:
							dupobj[obj] += 1
						else:
							dupobj[obj] = 2
						add_mod.append(wcsv_o)	

					if len(obj.outlist["u"])!=0:
						from nysol.mod.submod.writecsv import Nysol_Writecsv as mwritecsv
						wcsv_u = mwritecsv(obj.dlog+"_u")
						wcsv_u.inplist["i"]=[obj]
						obj.outlist["u"].append(wcsv_u)
						if obj in dupobj:
							dupobj[obj] += 1
						else:
							dupobj[obj] = 2
						add_mod.append(wcsv_u)	

		return add_mod		
	
	@classmethod
	def change_modNetworks(self,mods):
		sumiobj=set([])
		dupobj={}
		for mod in mods:
			mod.check_dupObj(sumiobj,dupobj)

		add_Dmod = NysolMOD_CORE.addIoMod(sumiobj,dupobj)

		#print("dup")
		#print(dupobj)
		#print("dup")
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
			#print("---------------")
			#print(obj)
			#print(obj.name)
			#print(obj.para2str())
			#print("i=",obj.inplist["i"])
			#print("m=",obj.inplist["m"])
			#print("o=",obj.outlist["o"])
			#print("u=",obj.outlist["u"])
			#print("---------------")
			modlist[no]= [obj.name,obj.para2str(),{},obj.tag]
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
	def runs(self,mods,**kw_args):

		msgF =False
		if "msg" in kw_args:
			if kw_args["msg"] == "on" :
				msgF = True

		stocks =[None]*len(mods)
		outfs = [None]*len(mods)

		for i,mod in enumerate(mods):
			if len(mod.outlist["o"]) != 0 :
				stocks[i] = mod.outlist["o"][0]
				
		dupobjs = copy.deepcopy(mods)

		#oが無ければlist出力追加
		runobjs =[None]*len(dupobjs)

		for i, dupobj in enumerate(dupobjs): 
			if len(dupobj.outlist["o"])==0:
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

		shobj = n_core.init(msgF)
		
		n_core.runL(shobj,modlist,linklist)
		return outfs

	def run(self,**kw_args):

		return NysolMOD_CORE.runs([self],**kw_args)[0]


	@classmethod
	def drawModelsCore(self,mod):
		dupshowobjs = copy.deepcopy(mod)
		showobjs =[]
		rtnlist = []
		for dupshowobj in dupshowobjs:
			if len(dupshowobj.outlist["o"])==0:
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
		return modlist,iolist,linklist
		
	#GRAPH表示 #deepコピーしてからチェック
	@classmethod
	def drawModels(self,mod,fname=None):

		modlist,iolist,linklist = NysolMOD_CORE.drawModelsCore(mod)	
		ndraw.chageSVG(modlist,iolist,linklist,fname)		

	#GRAPH表示 #deepコピーしてからチェック
	@classmethod
	def drawModelsD3(self,mod,fname=None):

		modlist,iolist,linklist = NysolMOD_CORE.drawModelsCore(mod)
		ndraw.chageSVG_D3(modlist,iolist,linklist,fname)		


	#GRAPH表示 #deepコピーしてからチェック
	def drawModel(self,fname=None):

		NysolMOD_CORE.drawModels([self],fname)


	def drawModelD3(self,fname=None):

		NysolMOD_CORE.drawModelsD3([self],fname)




	#GRAPH表示 #deepコピーしてからチェック
	@classmethod
	def modelInfos(self,mod):

		dupshowobjs = copy.deepcopy(mod)
		showobjs =[]
		rtnlist = []
		# 最終形式チェック
		for dupshowobj in dupshowobjs:
			if len(dupshowobj.outlist["o"])==0:
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


	def modelInfo(self):

		return NysolMOD_CORE.modelInfos([self])

	# 未実装
	def parallelrun(self,ilist,olist=None,num=2,**kw_args):

		if "msg" in kw_args:
			if kw_args["msg"] == "on" :
				self.msg = True


		listd = []
		runA = False
		sumiobj=set([])
		listd = self.make_modlist(sumiobj)

		runlist = []
		alist = listd
		while alist[2] != None:
			alist = alist[2] 

		chagVALi = copy.deepcopy(alist[1])
		chagVALo = copy.deepcopy(listd[1])
		rtnA = True if olist==None else False
		
		for i , fn in enumerate(ilist):
			if alist == listd:
				if olist == None :
					alist[1] = chagVALi + " i=" + fn
				else:
					alist[1] = chagVALi + " i=" + fn + " o=" + olist[i]
			else:		
				if olist == None :
					alist[1] = chagVALi + " i=" + fn
				else:
					alist[1]   = chagVALi + " i=" + fn 
					listd[1] = chagVALo + " o=" + olist[i]
			runlist.append([copy.deepcopy(listd),rtnA,self.msg])	

		p = Pool(num)
		output = p.map(mulit_run, runlist)
		p.close()

		return output

	# 子クラス生成　	# ここうっとしい いい方法が有れば変更
	def mfifo(self,*args, **kw_args):
		from nysol.mod.submod.mfifo import Nysol_Mfifo as mfifo
		return mfifo(nutil.args2dict(args,kw_args,mfifo.kwd)).addPre(self)

	def writecsv(self,*args, **kw_args):
		from nysol.mod.submod.writecsv import Nysol_Writecsv as writecsv
		return writecsv(nutil.args2dict(args,kw_args,writecsv.kwd,uk="o")).addPre(self)

	def writelist(self,*args, **kw_args):
		from nysol.mod.submod.writelist import Nysol_Writelist as writelist
		return writelist(nutil.args2dict(args,kw_args,writelist.kwd,uk="o")).addPre(self)

	def cmd(self,*args, **kw_args):
		from nysol.mod.submod.cmd import Nysol_Excmd as cmd
		return cmd(nutil.arg2dict(args,kw_args,cmd.kwd)).addPre(self)

	# 子クラス生成
	def m2cross(self,*args, **kw_args):
		from nysol.mod.submod.m2cross import Nysol_M2cross as m2cross
		return m2cross(nutil.args2dict(args,kw_args,m2cross.kwd)).addPre(self)


	def maccum(self,*args, **kw_args):
		from nysol.mod.submod.maccum import Nysol_Maccum as maccum
		return maccum(nutil.args2dict(args,kw_args,maccum.kwd)).addPre(self)

	def mavg(self,*args, **kw_args):
		from nysol.mod.submod.mavg import Nysol_Mavg as mavg
		return mavg(nutil.args2dict(args,kw_args,mavg.kwd)).addPre(self)

	def mbest(self,*args, **kw_args):
		from nysol.mod.submod.mbest import Nysol_Mbest as mbest
		return mbest(nutil.args2dict(args,kw_args,mbest.kwd)).addPre(self)

	def mbucket(self,*args, **kw_args):
		from nysol.mod.submod.mbucket import Nysol_Mbucket as mbucket
		return mbucket(nutil.args2dict(args,kw_args,mbucket.kwd)).addPre(self)

	def mchgnum(self,*args, **kw_args):
		from nysol.mod.submod.mchgnum import Nysol_Mchgnum as mchgnum
		return mchgnum(nutil.args2dict(args,kw_args,mchgnum.kwd)).addPre(self)

	def mchgstr(self,*args, **kw_args):
		from nysol.mod.submod.mchgstr import Nysol_Mchgstr as mchgstr
		return mchgstr(nutil.args2dict(args,kw_args,mchgstr.kwd)).addPre(self)

	def mcombi(self,*args, **kw_args):
		from nysol.mod.submod.mcombi import Nysol_Mcombi as mcombi
		return mcombi(nutil.args2dict(args,kw_args,mcombi.kwd)).addPre(self)

	def mcommon(self,*args, **kw_args):
		from nysol.mod.submod.mcommon import Nysol_Mcommon as mcommon
		return mcommon(nutil.args2dict(args,kw_args,mcommon.kwd)).addPre(self)

	def mcount(self,*args, **kw_args):
		from nysol.mod.submod.mcount import Nysol_Mcount as mcount
		return mcount(nutil.args2dict(args,kw_args,mcount.kwd)).addPre(self)

	def mcross(self,*args, **kw_args):
		from nysol.mod.submod.mcross import Nysol_Mcross as mcross
		return mcross(nutil.args2dict(args,kw_args,mcross.kwd)).addPre(self)

	def mdelnull(self,*args, **kw_args):
		from nysol.mod.submod.mdelnull import Nysol_Mdelnull as mdelnull
		return mdelnull(nutil.args2dict(args,kw_args,mdelnull.kwd)).addPre(self)

	def mdformat(self,*args, **kw_args):
		from nysol.mod.submod.mdformat import Nysol_Mdformat as mdformat
		return mdformat(nutil.args2dict(args,kw_args,mdformat.kwd)).addPre(self)

	def mduprec(self,*args, **kw_args):
		from nysol.mod.submod.mduprec import Nysol_Mduprec as mduprec
		return mduprec(nutil.args2dict(args,kw_args,mduprec.kwd)).addPre(self)

	def mfldname(self,*args, **kw_args):
		from nysol.mod.submod.mfldname import Nysol_Mfldname as mfldname
		return mfldname(nutil.args2dict(args,kw_args,mfldname.kwd)).addPre(self)

	def mfsort(self,*args, **kw_args):
		from nysol.mod.submod.mfsort import Nysol_Mfsort as mfsort
		return mfsort(nutil.args2dict(args,kw_args,mfsort.kwd)).addPre(self)

	def mhashavg(self,*args, **kw_args):
		from nysol.mod.submod.mhashavg import Nysol_Mhashavg as mhashavg
		return mhashavg(nutil.args2dict(args,kw_args,mhashavg.kwd)).addPre(self)

	def mhashsum(self,*args, **kw_args):
		from nysol.mod.submod.mhashsum import Nysol_Mhashavg as mhashsum
		return mhashsum(nutil.args2dict(args,kw_args,mhashsum.kwd)).addPre(self)

	def mkeybreak(self,*args, **kw_args):
		from nysol.mod.submod.mkeybreak import Nysol_Mkeybreak as mkeybreak
		return mkeybreak(nutil.args2dict(args,kw_args,mkeybreak.kwd)).addPre(self)

	def mmbucket(self,*args, **kw_args):
		from nysol.mod.submod.mmbucket import Nysol_Mmbucket as mmbucket
		return mmbucket(nutil.args2dict(args,kw_args,mmbucket.kwd)).addPre(self)

	def mmvavg(self,*args, **kw_args):
		from nysol.mod.submod.mmvavg import Nysol_Mmvavg as mmvavg
		return mmvavg(nutil.args2dict(args,kw_args,mmvavg.kwd)).addPre(self)

	def mmvsim(self,*args, **kw_args):
		from nysol.mod.submod.mmvsim import Nysol_Mmvsim as mmvsim
		return mmvsim(nutil.args2dict(args,kw_args,mmvsim.kwd)).addPre(self)

	def mmvstats(self,*args, **kw_args):
		from nysol.mod.submod.mmvstats import Nysol_Mmvstats as mmvstats
		return mmvstats(nutil.args2dict(args,kw_args,mmvstats.kwd)).addPre(self)

	def mnewnumber(self,*args, **kw_args):
		from nysol.mod.submod.mnewnumber import Nysol_Mnewnumber as mnewnumber
		return mnewnumber(nutil.args2dict(args,kw_args,mnewnumber.kwd)).addPre(self)

	def mnewrand(self,*args, **kw_args):
		from nysol.mod.submod.mnewrand import Nysol_Mnewrand as mnewrand
		return mnewrand(nutil.args2dict(args,kw_args,mnewrand.kwd)).addPre(self)

	def mnewstr(self,*args, **kw_args):
		from nysol.mod.submod.mnewstr import Nysol_Mnewstr as mnewstr
		return mnewstr(nutil.args2dict(args,kw_args,mnewstr.kwd)).addPre(self)

	def mnjoin(self,*args, **kw_args):
		from nysol.mod.submod.mnjoin import Nysol_Mnjoin as mnjoin
		return mnjoin(nutil.args2dict(args,kw_args,mnjoin.kwd)).addPre(self)

	def mnormalize(self,*args, **kw_args):
		from nysol.mod.submod.mnormalize import Nysol_Mnormalize as mnormalize
		return mnormalize(nutil.args2dict(args,kw_args,mnormalize.kwd)).addPre(self)

	def mnrcommon(self,*args, **kw_args):
		from nysol.mod.submod.mnrcommon import Nysol_Mnrcommon as mnrcommon
		return mnrcommon(nutil.args2dict(args,kw_args,mnrcommon.kwd)).addPre(self)

	def mnrjoin(self,*args, **kw_args):
		from nysol.mod.submod.mnrjoin import Nysol_Mnrjoin as mnrjoin
		return mnrjoin(nutil.args2dict(args,kw_args,mnrjoin.kwd)).addPre(self)

	def mnullto(self,*args, **kw_args):
		from nysol.mod.submod.mnullto import Nysol_Mnullto as mnullto
		return mnullto(nutil.args2dict(args,kw_args,mnullto.kwd)).addPre(self)

	def mnumber(self,*args, **kw_args):
		from nysol.mod.submod.mnumber import Nysol_Mnumber as mnumber
		return mnumber(nutil.args2dict(args,kw_args,mnumber.kwd)).addPre(self)

	def mpadding(self,*args, **kw_args):
		from nysol.mod.submod.mpadding import Nysol_Mpadding as mpadding
		return mpadding(nutil.args2dict(args,kw_args,mpadding.kwd)).addPre(self)

	def mpaste(self,*args, **kw_args):
		from nysol.mod.submod.mpaste import Nysol_Mpaste as mpaste
		return mpaste(nutil.args2dict(args,kw_args,mpaste.kwd)).addPre(self)

	def mproduct(self,*args, **kw_args):
		from nysol.mod.submod.mproduct import Nysol_Mproduct as mproduct
		return mproduct(nutil.args2dict(args,kw_args,mproduct.kwd)).addPre(self)

	def mrand(self,*args, **kw_args):
		from nysol.mod.submod.mrand import Nysol_Mrand as mrand
		return mrand(nutil.args2dict(args,kw_args,mrand.kwd)).addPre(self)

	def mrjoin(self,*args, **kw_args):
		from nysol.mod.submod.mrjoin import Nysol_Mrjoin as mrjoin
		return mrjoin(nutil.args2dict(args,kw_args,mrjoin.kwd)).addPre(self)

	def msed(self,*args, **kw_args):
		from nysol.mod.submod.msed import Nysol_Msed as msed
		return msed(nutil.args2dict(args,kw_args,msed.kwd)).addPre(self)

	def msel(self,*args, **kw_args):
		from nysol.mod.submod.msel import Nysol_Msel as msel
		return msel(nutil.args2dict(args,kw_args,msel.kwd)).addPre(self)

	def mselnum(self,*args, **kw_args):
		from nysol.mod.submod.mselnum import Nysol_Mselnum as mselnum
		return mselnum(nutil.args2dict(args,kw_args,mselnum.kwd)).addPre(self)

	def mselrand(self,*args, **kw_args):
		from nysol.mod.submod.mselrand import Nysol_Mselrand as mselrand
		return mselrand(nutil.args2dict(args,kw_args,mselrand.kwd)).addPre(self)

	def mselstr(self,*args, **kw_args):
		from nysol.mod.submod.mselstr import Nysol_Mselstr as mselstr
		return mselstr(nutil.args2dict(args,kw_args,mselstr.kwd)).addPre(self)

	def msetstr(self,*args, **kw_args):
		from nysol.mod.submod.msetstr import Nysol_Msetstr as msetstr
		return msetstr(nutil.args2dict(args,kw_args,msetstr.kwd)).addPre(self)

	def mshare(self,*args, **kw_args):
		from nysol.mod.submod.mshare import Nysol_Mshare as mshare
		return mshare(nutil.args2dict(args,kw_args,mshare.kwd)).addPre(self)

	def msim(self,*args, **kw_args):
		from nysol.mod.submod.msim import Nysol_Msim as msim
		return msim(nutil.args2dict(args,kw_args,msim.kwd)).addPre(self)

	def mslide(self,*args, **kw_args):
		from nysol.mod.submod.mslide import Nysol_Mslide as mslide
		return mslide(nutil.args2dict(args,kw_args,mslide.kwd)).addPre(self)

	def msplit(self,*args, **kw_args):
		from nysol.mod.submod.msplit import Nysol_Msplit as msplit
		return msplit(nutil.args2dict(args,kw_args,msplit.kwd)).addPre(self)

	def mstats(self,*args, **kw_args):
		from nysol.mod.submod.mstats import Nysol_Mstats as mstats
		return mstats(nutil.args2dict(args,kw_args,mstats.kwd)).addPre(self)

	def msummary(self,*args, **kw_args):
		from nysol.mod.submod.msummary import Nysol_Msummary as msummary
		return msummary(nutil.args2dict(args,kw_args,msummary.kwd)).addPre(self)

	def mtonull(self,*args, **kw_args):
		from nysol.mod.submod.mtonull import Nysol_Mtonull as mtonull
		return mtonull(nutil.args2dict(args,kw_args,mtonull.kwd)).addPre(self)

	def mtra(self,*args, **kw_args):
		from nysol.mod.submod.mtra import Nysol_Mtra as mtra
		return mtra(nutil.args2dict(args,kw_args,mtra.kwd)).addPre(self)

	def mtraflg(self,*args, **kw_args):
		from nysol.mod.submod.mtraflg import Nysol_Mtraflg as mtraflg
		return mtraflg(nutil.args2dict(args,kw_args,mtraflg.kwd)).addPre(self)

	def mtrafld(self,*args, **kw_args):
		from nysol.mod.submod.mtrafld import Nysol_Mtrafld as mtrafld
		return mtrafld(nutil.args2dict(args,kw_args,mtrafld.kwd)).addPre(self)

	def muniq(self,*args, **kw_args):
		from nysol.mod.submod.muniq import Nysol_Muniq as muniq
		return muniq(nutil.args2dict(args,kw_args,muniq.kwd)).addPre(self)

	def mvcat(self,*args, **kw_args):
		from nysol.mod.submod.mvcat import Nysol_Mvcat as mvcat
		return mvcat(nutil.args2dict(args,kw_args,mvcat.kwd)).addPre(self)

	def mvcommon(self,*args, **kw_args):
		from nysol.mod.submod.mvcommon import Nysol_Mvcommon as mvcommon
		return mvcommon(nutil.args2dict(args,kw_args,mvcommon.kwd)).addPre(self)

	def mvcount(self,*args, **kw_args):
		from nysol.mod.submod.mvcount import Nysol_Mvcount as mvcount
		return mvcount(nutil.args2dict(args,kw_args,mvcount.kwd)).addPre(self)

	def mvdelim(self,*args, **kw_args):
		from nysol.mod.submod.mvdelim import Nysol_Mvdelim as mvdelim
		return mvdelim(nutil.args2dict(args,kw_args,mvdelim.kwd)).addPre(self)

	def mvdelnull(self,*args, **kw_args):
		from nysol.mod.submod.mvdelnull import Nysol_Mvdelnull as mvdelnull
		return mvdelnull(nutil.args2dict(args,kw_args,mvdelnull.kwd)).addPre(self)

	def mvjoin(self,*args, **kw_args):
		from nysol.mod.submod.mvjoin import Nysol_Mvjoin as mvjoin
		return mvjoin(nutil.args2dict(args,kw_args,mvjoin.kwd)).addPre(self)

	def mvnullto(self,*args, **kw_args):
		from nysol.mod.submod.mvnullto import Nysol_Mvnullto as mvnullto
		return mvnullto(nutil.args2dict(args,kw_args,mvnullto.kwd)).addPre(self)

	def mvreplace(self,*args, **kw_args):
		from nysol.mod.submod.mvreplace import Nysol_Mvreplace as mvreplace
		return mvreplace(nutil.args2dict(args,kw_args,mvreplace.kwd)).addPre(self)

	def mvsort(self,*args, **kw_args):
		from nysol.mod.submod.mvsort import Nysol_Mvsort as mvsort
		return mvsort(nutil.args2dict(args,kw_args,mvsort.kwd)).addPre(self)

	def mvuniq(self,*args, **kw_args):
		from nysol.mod.submod.mvuniq import Nysol_Mvuniq as mvuniq
		return mvuniq(nutil.args2dict(args,kw_args,mvuniq.kwd)).addPre(self)

	def mwindow(self,*args, **kw_args):
		from nysol.mod.submod.mwindow import Nysol_Mwindow as mwindow
		return mwindow(nutil.args2dict(args,kw_args,mwindow.kwd)).addPre(self)

	def mcal(self,*args, **kw_args):
		from nysol.mod.submod.mcal import Nysol_Mcal as mcal
		return mcal(nutil.args2dict(args,kw_args,mcal.kwd)).addPre(self)

	def mcat(self,*args, **kw_args):
		from nysol.mod.submod.mcat import Nysol_Mcat as mcat
		return mcat(nutil.args2dict(args,kw_args,mcat.kwd)).addPre(self)

	def mcut(self,*args, **kw_args):
		from nysol.mod.submod.mcut import Nysol_Mcut as mcut
		return mcut(nutil.args2dict(args,kw_args,mcut.kwd)).addPre(self)

	def msum(self,*args, **kw_args):
		from nysol.mod.submod.msum import Nysol_Msum as msum
		return msum(nutil.args2dict(args,kw_args,msum.kwd)).addPre(self)

	def marff2csv(self,*args, **kw_args):
		from nysol.mod.submod.marff2csv import Nysol_Marff2csv as marff2csv
		return marff2csv(nutil.args2dict(args,kw_args,marff2csv.kwd)).addPre(self)

	def mtab2csv(self,*args, **kw_args):
		from nysol.mod.submod.mtab2csv import Nysol_Mtab2csv as mtab2csv
		return mtab2csv(nutil.args2dict(args,kw_args,mtab2csv.kwd)).addPre(self)

	def mxml2csv(self,*args, **kw_args):
		from nysol.mod.submod.mxml2csv import Nysol_Mxml2csv as mxml2csv
		return mxml2csv(nutil.args2dict(args,kw_args,mxml2csv.kwd)).addPre(self)

	def msortf(self,*args, **kw_args):
		from nysol.mod.submod.msortf import Nysol_Msortf as msortf
		return msortf(nutil.args2dict(args,kw_args,msortf.kwd)).addPre(self)

	def mjoin(self,*args, **kw_args):
		from nysol.mod.submod.mjoin import Nysol_Mjoin as mjoin
		return mjoin(nutil.args2dict(args,kw_args,mjoin.kwd)).addPre(self)


class Nysol_MeachIter(object):

	def __init__(self,obj):
		
		dupobj = copy.deepcopy(obj)

		if len(dupobj.outlist["o"])==0:
			runobj = dupobj
		else:
			print ("type ERORR")
			return None
			
		
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
		
		dupobj = copy.deepcopy(obj)

		if len(dupobj.outlist["o"])==0:
			from nysol.mod.submod.msortf import Nysol_Msortf as msortf
			sortkeys = copy.deepcopy(keys)
			if skeys != None:
				sortkeys.extend(skeys)
			
			runobj = msortf({"f":sortkeys}).addPre(dupobj)

		else:
			print ("type ERORR")
			return None
			
		
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
		self.csvin = n_core.runkeyiter(self.shobj,modlist,linklist,keys)

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


