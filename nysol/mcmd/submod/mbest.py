# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mbest(NysolMOD_CORE):

	_kwd ,_inkwd ,_outkwd = n_core.getparalist("mbest",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mbest,self).__init__("mbest",nutil.args2dict(args,kw_args,Nysol_Mbest._kwd))

def mbest(self,*args, **kw_args):
	return Nysol_Mbest(nutil.args2dict(args,kw_args,Nysol_Mbest._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mbest", mbest)

