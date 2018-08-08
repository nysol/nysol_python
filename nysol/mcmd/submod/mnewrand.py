# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mcmd.nysollib.core import NysolMOD_CORE
from nysol.mcmd.nysollib import nysolutil as nutil

class Nysol_Mnewrand(NysolMOD_CORE):

	_kwd ,_inkwd,_outkwd = n_core.getparalist("mnewrand",3)

	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnewrand,self).__init__("mnewrand",nutil.args2dict(args,kw_args,Nysol_Mnewrand._kwd))

