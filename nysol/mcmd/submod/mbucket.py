# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mbucket(NysolMOD_CORE):

	_kwd,_inkwd,_outkwd = n_core.getparalist("mbucket",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mbucket,self).__init__("mbucket",nutil.args2dict(args,kw_args,Nysol_Mbucket._kwd))

def mbucket(self,*args, **kw_args):
	return Nysol_Mbucket(nutil.args2dict(args,kw_args,Nysol_Mbucket._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mbucket", mbucket)

