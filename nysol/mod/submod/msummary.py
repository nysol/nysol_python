# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Msummary(NysolMOD_CORE):
	kwd = n_core.getparalist("msummary")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Msummary,self).__init__("msummary",nutil.args2dict(args,kw_args,Nysol_Msummary.kwd))

def msummary(self,*args, **kw_args):
	return Nysol_Msummary(nutil.args2dict(args,kw_args,Nysol_Msummary.kwd)).addPre(self)

setattr(NysolMOD_CORE, "msummary", msummary)
