# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Msortchk(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("msortchk",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Msortchk,self).__init__("msortchk",nutil.args2dict(args,kw_args,Nysol_Msortchk._kwd))


def msortchk(self,*args, **kw_args):
	return Nysol_Msortchk(nutil.args2dict(args,kw_args,Nysol_Msortchk._kwd)).addPre(self)

setattr(NysolMOD_CORE, "msortchk", msortchk)
