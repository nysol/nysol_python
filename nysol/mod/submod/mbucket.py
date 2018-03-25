# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.core import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mbucket(NysolMOD_CORE):
	kwd = n_core.getparalist("mbucket")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mbucket,self).__init__("mbucket",nutil.args2dict(args,kw_args,Nysol_Mbucket.kwd))

def mbucket(self,*args, **kw_args):
	return Nysol_Mbucket(nutil.args2dict(args,kw_args,Nysol_Mbucket.kwd)).addPre(self)

setattr(NysolMOD_CORE, "mbucket", mbucket)

