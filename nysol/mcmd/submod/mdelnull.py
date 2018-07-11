# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mdelnull(NysolMOD_CORE):
	kwd = n_core.getparalist("mdelnull")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mdelnull,self).__init__("mdelnull",nutil.args2dict(args,kw_args,Nysol_Mdelnull.kwd))


def mdelnull(self,*args, **kw_args):
	return Nysol_Mdelnull(nutil.args2dict(args,kw_args,Nysol_Mdelnull.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mdelnull", mdelnull)
