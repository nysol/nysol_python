# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Msplit(NysolMOD_CORE):
	kwd = n_core.getparalist("msplit")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Msplit,self).__init__("msplit",nutil.args2dict(args,kw_args,Nysol_Msplit.kwd))


def msplit(self,*args, **kw_args):
	return Nysol_Msplit(nutil.args2dict(args,kw_args,Nysol_Msplit.kwd)).addPre(self)

setattr(NysolMOD_CORE, "msplit", msplit)
