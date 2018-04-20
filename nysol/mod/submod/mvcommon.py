# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mvcommon(NysolMOD_CORE):
	kwd = n_core.getparalist("mvcommon")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvcommon,self).__init__("mvcommon",nutil.args2dict(args,kw_args,Nysol_Mvcommon.kwd))

def mvcommon(self,*args, **kw_args):
	return Nysol_Mvcommon(nutil.args2dict(args,kw_args,Nysol_Mvcommon.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvcommon", mvcommon)
