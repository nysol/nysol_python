# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mproduct(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("mproduct",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mproduct,self).__init__("mproduct",nutil.args2dict(args,kw_args,Nysol_Mproduct._kwd))


def mproduct(self,*args, **kw_args):
	return Nysol_Mproduct(nutil.args2dict(args,kw_args,Nysol_Mproduct._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mproduct", mproduct)
