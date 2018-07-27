# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mtonull(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mtonull",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mtonull,self).__init__("mtonull",nutil.args2dict(args,kw_args,Nysol_Mtonull._kwd))

def mtonull(self,*args, **kw_args):
	return Nysol_Mtonull(nutil.args2dict(args,kw_args,Nysol_Mtonull._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mtonull", mtonull)
