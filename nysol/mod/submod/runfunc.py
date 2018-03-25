# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

def _para2dict(func,args,kw_args):
	params = {}
	params["runfunc_func"] = func 
	params["runfunc_args"] = args
	params["runfunc_kwd"]  = kw_args
	return params


class Nysol_Runfunc(NysolMOD_CORE):
	kwd = n_core.getparalist("runfunc")
	def __init__(self,func,*args, **kw_args) :
		super(Nysol_Runfunc,self).__init__("runfunc",_para2dict(func,args,kw_args))


def runfunc(self,func,*args, **kw_args):
	return Nysol_Runfunc(_para2dict(func,args,kw_args)).addPre(self)

setattr(NysolMOD_CORE, "runfunc", runfunc)
