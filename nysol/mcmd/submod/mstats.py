# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mstats(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mstats",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mstats,self).__init__("mstats",nutil.args2dict(args,kw_args,Nysol_Mstats._kwd))

def mstats(self,*args, **kw_args):
	return Nysol_Mstats(nutil.args2dict(args,kw_args,Nysol_Mstats._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mstats", mstats)
