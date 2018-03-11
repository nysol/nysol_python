# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mcommon(NysolMOD_CORE):
	kwd = n_core.getparalist("mcommon")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mcommon,self).__init__("mcommon",nutil.args2dict(args,kw_args,Nysol_Mcommon.kwd))

def mcommon(self,*args, **kw_args):
	return Nysol_Mcommon(nutil.args2dict(args,kw_args,Nysol_Mcommon.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mcommon", mcommon)
