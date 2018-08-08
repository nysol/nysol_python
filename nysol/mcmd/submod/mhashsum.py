# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mhashsum(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mhashsum",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mhashsum,self).__init__("mhashsum",nutil.args2dict(args,kw_args,Nysol_Mhashsum._kwd))


def mhashsum(self,*args, **kw_args):
	return Nysol_Mhashsum(nutil.args2dict(args,kw_args,Nysol_Mhashsum._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mhashsum", mhashsum)
