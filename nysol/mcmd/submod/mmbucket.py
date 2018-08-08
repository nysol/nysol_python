# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mmbucket(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mmbucket",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mmbucket,self).__init__("mmbucket",nutil.args2dict(args,kw_args,Nysol_Mmbucket._kwd))

def mmbucket(self,*args, **kw_args):
	return Nysol_Mmbucket(nutil.args2dict(args,kw_args,Nysol_Mmbucket._kwd)).addPre(self)

setattr(NysolMOD_CORE, "mmbucket", mmbucket)
