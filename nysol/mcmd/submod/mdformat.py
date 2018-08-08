# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mdformat(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mdformat",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mdformat,self).__init__("mdformat",nutil.args2dict(args,kw_args,Nysol_Mdformat._kwd))

def mdformat(self,*args, **kw_args):
	return Nysol_Mdformat(nutil.args2dict(args,kw_args,Nysol_Mdformat._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mdformat", mdformat)
