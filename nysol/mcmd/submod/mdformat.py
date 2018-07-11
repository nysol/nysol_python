# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mdformat(NysolMOD_CORE):
	kwd = n_core.getparalist("mdformat")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mdformat,self).__init__("mdformat",nutil.args2dict(args,kw_args,Nysol_Mdformat.kwd))

def mdformat(self,*args, **kw_args):
	return Nysol_Mdformat(nutil.args2dict(args,kw_args,Nysol_Mdformat.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mdformat", mdformat)
