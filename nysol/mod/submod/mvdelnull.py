# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mvdelnull(NysolMOD_CORE):
	kwd = n_core.getparalist("mvdelnull")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mvdelnull,self).__init__("mvdelnull",nutil.args2dict(args,kw_args,Nysol_Mvdelnull.kwd))


def mvdelnull(self,*args, **kw_args):
	return Nysol_Mvdelnull(nutil.args2dict(args,kw_args,Nysol_Mvdelnull.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mvdelnull", mvdelnull)
