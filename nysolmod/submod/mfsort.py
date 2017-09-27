# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Mfsort(NysolMOD_CORE):
	kwd = n_core.getparalist("mfsort")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mfsort,self).__init__("mfsort",nutil.args2dict(args,kw_args,Nysol_Mfsort.kwd))

