# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mtonull(NysolMOD_CORE):
	kwd = n_core.getparalist("mtonull")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mtonull,self).__init__("mtonull",nutil.args2dict(args,kw_args,Nysol_Mtonull.kwd))

def mtonull(self,*args, **kw_args):
	return Nysol_Mtonull(nutil.args2dict(args,kw_args,Nysol_Mtonull.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mtonull", mtonull)
