# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_M2tee(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("m2tee",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_M2tee,self).__init__("m2tee",nutil.args2dict(args,kw_args,Nysol_M2tee._kwd))


def m2tee(self,*args, **kw_args):
	return Nysol_M2tee(nutil.args2dict(args,kw_args,Nysol_M2tee._kwd)).addPre(self)

setattr(NysolMOD_CORE, "m2tee", m2tee)


