# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mjoin(NysolMOD_CORE):
	kwd = n_core.getparalist("mjoin")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mjoin,self).__init__("mjoin",nutil.args2dict(args,kw_args,Nysol_Mjoin.kwd))

def mjoin(self,*args, **kw_args):
	return Nysol_Mjoin(nutil.args2dict(args,kw_args,Nysol_Mjoin.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mjoin", mjoin)
