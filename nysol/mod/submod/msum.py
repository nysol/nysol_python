# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Msum(NysolMOD_CORE):
	kwd = n_core.getparalist("msum")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Msum,self).__init__("msum",nutil.args2dict(args,kw_args,Nysol_Msum.kwd))
