# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_M2cat(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd  = n_core.getparalist("m2cat",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_M2cat,self).__init__("m2cat",nutil.args2dict(args,kw_args,Nysol_M2cat._kwd))

def m2cat(self,*args, **kw_args):
	return Nysol_M2cat(nutil.args2dict(args,kw_args,Nysol_M2cat._kwd)).addPre(self)

setattr(NysolMOD_CORE, "m2cat", m2cat)

