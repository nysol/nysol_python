# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mshare(NysolMOD_CORE):
	kwd = n_core.getparalist("mshare")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mshare,self).__init__("mshare",nutil.args2dict(args,kw_args,Nysol_Mshare.kwd))


def mshare(self,*args, **kw_args):
	return Nysol_Mshare(nutil.args2dict(args,kw_args,Nysol_Mshare.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mshare", mshare)
