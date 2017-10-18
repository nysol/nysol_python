# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_Mproduct(NysolMOD_CORE):
	kwd = n_core.getparalist("mproduct")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mproduct,self).__init__("mproduct",nutil.args2dict(args,kw_args,Nysol_Mproduct.kwd))

