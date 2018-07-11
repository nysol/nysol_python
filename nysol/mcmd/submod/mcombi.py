# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mcombi(NysolMOD_CORE):
	kwd = n_core.getparalist("mcombi")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mcombi,self).__init__("mcombi",nutil.args2dict(args,kw_args,Nysol_Mcombi.kwd))

def mcombi(self,*args, **kw_args):
	return Nysol_Mcombi(nutil.args2dict(args,kw_args,Nysol_Mcombi.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mcombi", mcombi)

