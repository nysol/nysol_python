# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mcross(NysolMOD_CORE):
	kwd = n_core.getparalist("mcross")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mcross,self).__init__("mcross",nutil.args2dict(args,kw_args,Nysol_Mcross.kwd))

def mcross(self,*args, **kw_args):
	return Nysol_Mcross(nutil.args2dict(args,kw_args,Nysol_Mcross.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mcross", mcross)
