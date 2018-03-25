# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mfsort(NysolMOD_CORE):
	kwd = n_core.getparalist("mfsort")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mfsort,self).__init__("mfsort",nutil.args2dict(args,kw_args,Nysol_Mfsort.kwd))

def mfsort(self,*args, **kw_args):
	return Nysol_Mfsort(nutil.args2dict(args,kw_args,Nysol_Mfsort.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mfsort", mfsort)
