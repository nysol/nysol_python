# -*- coding: utf-8 -*-
import shlex
import re
import _nysolshell_core as n_core
import nysolmod.nysollib.nysolutil as nutil
import nysolmod as nsub

from multiprocessing import Pool
import copy
import os, sys


def mulit_run(val):
	cc = n_core.init(False)
	return n_core.run(cc,val[0],val[1])

class NysolMOD_CORE(object):
	# i,o,m,uは別処理
	def __init__(self,name=None,kwd=None) :
		if kwd == None:
			kwd = {}
		self.name = name
		self.kwd   = kwd

		self.inp   = kwd.get("i",None)
		if "i" in self.kwd :
			del self.kwd["i"]

		self.outp  = kwd.get("o",None)
		if "o" in self.kwd :
			del self.kwd["o"]
		
		self.refp  = kwd.get("m",None)
		if "m" in self.kwd :
			del self.kwd["m"]

		#self.unp = kwd.get("u",None)
		#if "u" in self.kwd :
		#	del self.kwd["u"]

	def __iter__(self):
		return Nysol_MeachIter(self)


	def addPre(self,pre):
		self.inp = pre
		return self

	# PRIVATEにする？
	def para2str(self):
		rtnStr = ""
		#print self.kwd
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
				
		return rtnStr
	
	def params(self,*args, **kw_args):
		self.kwd.update(nutil.args2dict(args,kw_args,self.kwd))

	def make_modlist(self):
		i0 = None
		i1 = None
		if isinstance(self.inp,NysolMOD_CORE):
			i0 = self.inp.make_modlist() 
		elif isinstance(self.inp,list):
			i0 = self.inp
		elif isinstance(self.inp,str):
			i0 = self.inp


		if isinstance(self.refp,NysolMOD_CORE):
			i1 = self.refp.make_modlist() 
		elif isinstance(self.refp,list):
			i1 = self.refp
		elif isinstance(self.refp,str):
			i1 = self.refp

		return [self.name,self.para2str(),i0,i1]

	def run(self,**kw_args):
		msgFlg = False
		if "msg" in kw_args:
			if kw_args["msg"] == "on" :
				msgFlg = True

		listd = []
		runA = False
		outf = self.outp
		if not isinstance(outf,str):
			outf = None
			runA = True 

		listd = self.make_modlist()
		shobj = n_core.init(msgFlg)

		if runA:
			return n_core.run(shobj,listd,runA)
		else:
			listd[1] += " o=" + outf 
			n_core.run(shobj,listd,runA)
			return outf


	def show(self):
		listd = []
		runA = False
		outf = self.outp
		if not isinstance(outf,str):
			outf = None
			runA = True 
		listd = self.make_modlist()
		print(listd)


	def parallelrun(self,ilist,olist=None,num=2,**kw_args):
		listd = []
		runA = False

		listd = self.make_modlist()

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
			runlist.append([copy.deepcopy(listd),rtnA])	

		p = Pool(num)
		output = p.map(mulit_run, runlist)
		p.close()

		return output

	# 子クラス生成
	#def add(self, obj):
	#	obj.inp = self
	#	self.obj = obj
	#	return obj

	# 子クラス生成
	def mload(self,*args, **kw_args):
		return nsub.mload(nutil.args2dict(args,kw_args,nsub.mload.kwd)).addPre(self)

	def msave(self,*args, **kw_args):
		return nsub.msave(nutil.args2dict(args,kw_args,nsub.msave.kwd)).addPre(self)

	# 子クラス生成
	def m2cross(self,*args, **kw_args):
		return nsub.m2cross(nutil.args2dict(args,kw_args,nsub.m2cross.kwd)).addPre(self)

	def maccum(self,*args, **kw_args):
		return nsub.maccum(nutil.args2dict(args,kw_args,nsub.maccum.kwd)).addPre(self)

	def mavg(self,*args, **kw_args):
		return nsub.mavg(nutil.args2dict(args,kw_args,nsub.mavg.kwd)).addPre(self)

	def mbest(self,*args, **kw_args):
		return nsub.mbest(nutil.args2dict(args,kw_args,nsub.mbest.kwd)).addPre(self)

	def mbucket(self,*args, **kw_args):
		return nsub.mbucket(nutil.args2dict(args,kw_args,nsub.mbucket.kwd)).addPre(self)

	def mchgnum(self,*args, **kw_args):
		return nsub.mchgnum(nutil.args2dict(args,kw_args,nsub.mchgnum.kwd)).addPre(self)

	def mchgstr(self,*args, **kw_args):
		return nsub.mchgstr(nutil.args2dict(args,kw_args,nsub.mchgstr.kwd)).addPre(self)

	def mcombi(self,*args, **kw_args):
		return nsub.mcombi(nutil.args2dict(args,kw_args,nsub.mcombi.kwd)).addPre(self)

	def mcommon(self,*args, **kw_args):
		return nsub.mcommon(nutil.args2dict(args,kw_args,nsub.mcommon.kwd)).addPre(self)

	def mcount(self,*args, **kw_args):
		return nsub.mcount(nutil.args2dict(args,kw_args,nsub.mcount.kwd)).addPre(self)

	def mcross(self,*args, **kw_args):
		return nsub.mcross(nutil.args2dict(args,kw_args,nsub.mcross.kwd)).addPre(self)

	def mdelnull(self,*args, **kw_args):
		return nsub.mdelnull(nutil.args2dict(args,kw_args,nsub.mdelnull.kwd)).addPre(self)

	def mdformat(self,*args, **kw_args):
		return nsub.mdformat(nutil.args2dict(args,kw_args,nsub.mdformat.kwd)).addPre(self)

	def mduprec(self,*args, **kw_args):
		return nsub.mduprec(nutil.args2dict(args,kw_args,nsub.mduprec.kwd)).addPre(self)

	def mfldname(self,*args, **kw_args):
		return nsub.mfldname(nutil.args2dict(args,kw_args,nsub.mfldname.kwd)).addPre(self)

	def mfsort(self,*args, **kw_args):
		return nsub.mfsort(nutil.args2dict(args,kw_args,nsub.mfsort.kwd)).addPre(self)

	def mhashavg(self,*args, **kw_args):
		return nsub.mhashavg(nutil.args2dict(args,kw_args,nsub.mhashavg.kwd)).addPre(self)

	def mhashsum(self,*args, **kw_args):
		return nsub.mhashsum(nutil.args2dict(args,kw_args,nsub.mhashsum.kwd)).addPre(self)

	def mkeybreak(self,*args, **kw_args):
		return nsub.mkeybreak(nutil.args2dict(args,kw_args,nsub.mkeybreak.kwd)).addPre(self)

	def mmbucket(self,*args, **kw_args):
		return nsub.mmbucket(nutil.args2dict(args,kw_args,nsub.mmbucket.kwd)).addPre(self)

	def mmvavg(self,*args, **kw_args):
		return nsub.mmvavg(nutil.args2dict(args,kw_args,nsub.mmvavg.kwd)).addPre(self)

	def mmvsim(self,*args, **kw_args):
		return nsub.mmvsim(nutil.args2dict(args,kw_args,nsub.mmvsim.kwd)).addPre(self)

	def mmvstats(self,*args, **kw_args):
		return nsub.mmvstats(nutil.args2dict(args,kw_args,nsub.mmvstats.kwd)).addPre(self)

	def mnewnumber(self,*args, **kw_args):
		return nsub.mnewnumber(nutil.args2dict(args,kw_args,nsub.mnewnumber.kwd)).addPre(self)

	def mnewrand(self,*args, **kw_args):
		return nsub.mnewrand(nutil.args2dict(args,kw_args,nsub.mnewrand.kwd)).addPre(self)

	def mnewstr(self,*args, **kw_args):
		return nsub.mnewstr(nutil.args2dict(args,kw_args,nsub.mnewstr.kwd)).addPre(self)

	def mnjoin(self,*args, **kw_args):
		return nsub.mnjoin(nutil.args2dict(args,kw_args,nsub.mnjoin.kwd)).addPre(self)

	def mnormalize(self,*args, **kw_args):
		return nsub.mnormalize(nutil.args2dict(args,kw_args,nsub.mnormalize.kwd)).addPre(self)

	def mnrcommon(self,*args, **kw_args):
		return nsub.mnrcommon(nutil.args2dict(args,kw_args,nsub.mnrcommon.kwd)).addPre(self)

	def mnrjoin(self,*args, **kw_args):
		return nsub.mnrjoin(nutil.args2dict(args,kw_args,nsub.mnrjoin.kwd)).addPre(self)

	def mnullto(self,*args, **kw_args):
		return nsub.mnullto(nutil.args2dict(args,kw_args,nsub.mnullto.kwd)).addPre(self)

	def mnumber(self,*args, **kw_args):
		return nsub.mnumber(nutil.args2dict(args,kw_args,nsub.mnumber.kwd)).addPre(self)

	def mpadding(self,*args, **kw_args):
		return nsub.mpadding(nutil.args2dict(args,kw_args,nsub.mpadding.kwd)).addPre(self)

	def mpaste(self,*args, **kw_args):
		return nsub.mpaste(nutil.args2dict(args,kw_args,nsub.mpaste.kwd)).addPre(self)

	def mproduct(self,*args, **kw_args):
		return nsub.mproduct(nutil.args2dict(args,kw_args,nsub.mproduct.kwd)).addPre(self)

	def mrand(self,*args, **kw_args):
		return nsub.mrand(nutil.args2dict(args,kw_args,nsub.mrand.kwd)).addPre(self)

	def mrjoin(self,*args, **kw_args):
		return nsub.mrjoin(nutil.args2dict(args,kw_args,nsub.mrjoin.kwd)).addPre(self)

	def msed(self,*args, **kw_args):
		return nsub.msed(nutil.args2dict(args,kw_args,nsub.msed.kwd)).addPre(self)

	def msel(self,*args, **kw_args):
		return nsub.msel(nutil.args2dict(args,kw_args,nsub.msel.kwd)).addPre(self)

	def mselnum(self,*args, **kw_args):
		return nsub.mselnum(nutil.args2dict(args,kw_args,nsub.mselnum.kwd)).addPre(self)

	def mselrand(self,*args, **kw_args):
		return nsub.mselrand(nutil.args2dict(args,kw_args,nsub.mselrand.kwd)).addPre(self)

	def mselstr(self,*args, **kw_args):
		return nsub.mselstr(nutil.args2dict(args,kw_args,nsub.mselstr.kwd)).addPre(self)

	def msetstr(self,*args, **kw_args):
		return nsub.msetstr(nutil.args2dict(args,kw_args,nsub.msetstr.kwd)).addPre(self)

	def mshare(self,*args, **kw_args):
		return nsub.mshare(nutil.args2dict(args,kw_args,nsub.mshare.kwd)).addPre(self)

	def msim(self,*args, **kw_args):
		return nsub.msim(nutil.args2dict(args,kw_args,nsub.msim.kwd)).addPre(self)

	def mslide(self,*args, **kw_args):
		return nsub.mslide(nutil.args2dict(args,kw_args,nsub.mslide.kwd)).addPre(self)

	def msplit(self,*args, **kw_args):
		return nsub.msplit(nutil.args2dict(args,kw_args,nsub.msplit.kwd)).addPre(self)

	def mstats(self,*args, **kw_args):
		return nsub.mstats(nutil.args2dict(args,kw_args,nsub.mstats.kwd)).addPre(self)

	def msummary(self,*args, **kw_args):
		return nsub.msummary(nutil.args2dict(args,kw_args,nsub.msummary.kwd)).addPre(self)

	def mtonull(self,*args, **kw_args):
		return nsub.mtonull(nutil.args2dict(args,kw_args,nsub.mtonull.kwd)).addPre(self)

	def mtra(self,*args, **kw_args):
		return nsub.mtra(nutil.args2dict(args,kw_args,nsub.mtra.kwd)).addPre(self)

	def mtraflg(self,*args, **kw_args):
		return nsub.mtraflg(nutil.args2dict(args,kw_args,nsub.mtraflg.kwd)).addPre(self)

	def muniq(self,*args, **kw_args):
		return nsub.muniq(nutil.args2dict(args,kw_args,nsub.muniq.kwd)).addPre(self)

	def mvcat(self,*args, **kw_args):
		return nsub.mvcat(nutil.args2dict(args,kw_args,nsub.mvcat.kwd)).addPre(self)

	def mvcommon(self,*args, **kw_args):
		return nsub.mvcommon(nutil.args2dict(args,kw_args,nsub.mvcommon.kwd)).addPre(self)

	def mvcount(self,*args, **kw_args):
		return nsub.mvcount(nutil.args2dict(args,kw_args,nsub.mvcount.kwd)).addPre(self)

	def mvdelim(self,*args, **kw_args):
		return nsub.mvdelim(nutil.args2dict(args,kw_args,nsub.mvdelim.kwd)).addPre(self)

	def mvdelnull(self,*args, **kw_args):
		return nsub.mvdelnull(nutil.args2dict(args,kw_args,nsub.mvdelnull.kwd)).addPre(self)

	def mvjoin(self,*args, **kw_args):
		return nsub.mvjoin(nutil.args2dict(args,kw_args,nsub.mvjoin.kwd)).addPre(self)

	def mvnullto(self,*args, **kw_args):
		return nsub.mvnullto(nutil.args2dict(args,kw_args,nsub.mvnullto.kwd)).addPre(self)

	def mvreplace(self,*args, **kw_args):
		return nsub.mvreplace(nutil.args2dict(args,kw_args,nsub.mvreplace.kwd)).addPre(self)

	def mvsort(self,*args, **kw_args):
		return nsub.mvsort(nutil.args2dict(args,kw_args,nsub.mvsort.kwd)).addPre(self)

	def mvuniq(self,*args, **kw_args):
		return nsub.mvuniq(nutil.args2dict(args,kw_args,nsub.mvuniq.kwd)).addPre(self)

	def mwindow(self,*args, **kw_args):
		return nsub.mwindow(nutil.args2dict(args,kw_args,nsub.mwindow.kwd)).addPre(self)

	def mcal(self,*args, **kw_args):
		return nsub.mcal(nutil.args2dict(args,kw_args,nsub.mcal.kwd)).addPre(self)

	def mcat(self,*args, **kw_args):
		return nsub.mcat(nutil.args2dict(args,kw_args,nsub.mcat.kwd)).addPre(self)

	def mcut(self,*args, **kw_args):
		return nsub.mcut(nutil.args2dict(args,kw_args,nsub.mcut.kwd)).addPre(self)

	def msum(self,*args, **kw_args):
		return nsub.msum(nutil.args2dict(args,kw_args,nsub.msum.kwd)).addPre(self)

	def marff2csv(self,*args, **kw_args):
		return nsub.marff2csv(nutil.args2dict(args,kw_args,nsub.marff2csv.kwd)).addPre(self)

	def mtab2csv(self,*args, **kw_args):
		return nsub.mtab2csv(nutil.args2dict(args,kw_args,nsub.mtab2csv.kwd)).addPre(self)

	def mxml2csv(self,*args, **kw_args):
		return nsub.mxml2csv(nutil.args2dict(args,kw_args,nsub.mxml2csv.kwd)).addPre(self)

	def msortf(self,*args, **kw_args):
		return nsub.msortf(nutil.args2dict(args,kw_args,nsub.msortf.kwd)).addPre(self)

	def mjoin(self,*args, **kw_args):
		return nsub.mjoin(nutil.args2dict(args,kw_args,nsub.mjoin.kwd)).addPre(self)



#IOから調節にした方がいい（kgshellをpython objにする？）
class Nysol_MeachIter(object):

	def __init__(self,obj):
		listd = []
		runA = False
		outf = obj.outp
		if not isinstance(outf,str):
			outf = None
			runA = True 

		i0 = None
		i1 = None

		if isinstance(obj.inp,NysolMOD_CORE):
			i0 = obj.inp.make_modlist() 
		elif isinstance(obj.inp,list):
			i0 = obj.inp
		elif isinstance(obj.inp,str):
			i0 = obj.inp


		if isinstance(obj.refp,NysolMOD_CORE):
			i1 = obj.refp.make_modlist() 
		elif isinstance(obj.refp,list):
			i1 = obj.refp
		elif isinstance(obj.refp,str):
			i1 = obj.refp

		listd = [obj.name,obj.para2str(),i0,i1]
		#print list
		self.shobj = n_core.init(False)
		if runA:
			self.csvin = n_core.runiter(self.shobj,listd,runA)
		else:
			#n_core.runiter(shobj,list,runA)
			#return outf
			pass

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

#class Nysol_Meach(NysolMOD_CORE):
#	kwd = {"-nfn","-nfni","-nfno","i=","o="}
#	def __init__(self, kwd,pre=None) :
#		super(Nysol_Meach,self).__init__("meach",kwd,pre)


