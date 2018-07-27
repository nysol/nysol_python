# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mnjoin(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mnjoin",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnjoin,self).__init__("mnjoin",nutil.args2dict(args,kw_args,Nysol_Mnjoin._kwd))

def mnjoin(self,*args, **kw_args):
	return Nysol_Mnjoin(nutil.args2dict(args,kw_args,Nysol_Mnjoin._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mnjoin", mnjoin)
