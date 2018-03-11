# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mnjoin(NysolMOD_CORE):
	kwd = n_core.getparalist("mnjoin")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnjoin,self).__init__("mnjoin",nutil.args2dict(args,kw_args,Nysol_Mnjoin.kwd))

def mnjoin(self,*args, **kw_args):
	return Nysol_Mnjoin(nutil.args2dict(args,kw_args,Nysol_Mnjoin.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mnjoin", mnjoin)
