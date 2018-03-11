# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mhashsum(NysolMOD_CORE):
	kwd = n_core.getparalist("mhashsum")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mhashsum,self).__init__("mhashsum",nutil.args2dict(args,kw_args,Nysol_Mhashsum.kwd))


def mhashsum(self,*args, **kw_args):
	return Nysol_Mhashsum(nutil.args2dict(args,kw_args,Nysol_Mhashsum.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mhashsum", mhashsum)
