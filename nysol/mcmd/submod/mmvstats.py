# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mmvstats(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mmvstats",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mmvstats,self).__init__("mmvstats",nutil.args2dict(args,kw_args,Nysol_Mmvstats._kwd))

def mmvstats(self,*args, **kw_args):
	return Nysol_Mmvstats(nutil.args2dict(args,kw_args,Nysol_Mmvstats._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mmvstats", mmvstats)
