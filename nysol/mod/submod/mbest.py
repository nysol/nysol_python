# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mbest(NysolMOD_CORE):
	kwd = n_core.getparalist("mbest")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mbest,self).__init__("mbest",nutil.args2dict(args,kw_args,Nysol_Mbest.kwd))

def mbest(self,*args, **kw_args):
	return Nysol_Mbest(nutil.args2dict(args,kw_args,Nysol_Mbest.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mbest", mbest)

