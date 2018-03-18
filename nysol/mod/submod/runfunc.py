# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Runfunc(NysolMOD_CORE):
	kwd = n_core.getparalist("runfunc")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Runfunc,self).__init__("runfunc",nutil.arg2dict(args,kw_args,Nysol_Runfunc.kwd))


def runfunc(self,*args, **kw_args):
	return Nysol_Runfunc(nutil.args2dict(args,kw_args,Nysol_Runfunc.kwd)).addPre(self)

setattr(NysolMOD_CORE, "runfunc", runfunc)
