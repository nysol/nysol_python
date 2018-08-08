# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mwrite(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd  = n_core.getparalist("mwrite",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mwrite,self).__init__("mwrite",nutil.args2dict(args,kw_args,Nysol_Mwrite._kwd))

def mwrite(self,*args, **kw_args):
	return Nysol_Mwrite(nutil.args2dict(args,kw_args,Nysol_Mwrite._kwd)).addPre(self)


setattr(NysolMOD_CORE, "mwrite", mwrite)
