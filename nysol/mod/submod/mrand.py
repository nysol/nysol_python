# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mrand(NysolMOD_CORE):
	kwd = n_core.getparalist("mrand")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mrand,self).__init__("mrand",nutil.args2dict(args,kw_args,Nysol_Mrand.kwd))


def mrand(self,*args, **kw_args):
	return Nysol_Mrand(nutil.args2dict(args,kw_args,Nysol_Mrand.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mrand", mrand)
