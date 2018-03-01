# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Msep(NysolMOD_CORE):
	kwd = n_core.getparalist("msep")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Msep,self).__init__("msep",nutil.args2dict(args,kw_args,Nysol_Msep.kwd))


def msep(self,*args, **kw_args):
	return Nysol_Msep(nutil.args2dict(args,kw_args,Nysol_Msep.kwd)).addPre(self)

setattr(NysolMOD_CORE, "msep", msep)
