# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Maccum(NysolMOD_CORE):

	_kwd ,_inkwd ,_outkwd = n_core.getparalist("maccum",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Maccum,self).__init__("maccum",nutil.args2dict(args,kw_args,Nysol_Maccum._kwd))

def maccum(self,*args, **kw_args):
	return Nysol_Maccum(nutil.args2dict(args,kw_args,Nysol_Maccum._kwd)).addPre(self)

setattr(NysolMOD_CORE, "maccum", maccum)

