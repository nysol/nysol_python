# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mselrand(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mselrand",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mselrand,self).__init__("mselrand",nutil.args2dict(args,kw_args,Nysol_Mselrand._kwd))

def mselrand(self,*args, **kw_args):
	return Nysol_Mselrand(nutil.args2dict(args,kw_args,Nysol_Mselrand._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mselrand", mselrand)
