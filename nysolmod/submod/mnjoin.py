# -*- coding: utf-8 -*-
import _nysolshell_core as n_core
from nysolmod.nysollib.nysolmodcore import NysolMOD_CORE
import nysolmod.nysollib.nysolutil as nutil

class Nysol_Mnjoin(NysolMOD_CORE):
	kwd = n_core.getparalist("mnjoin")
	def __init__(self,*args, **kw_args) :
		super(Nysol_Mnjoin,self).__init__("mnjoin",nutil.args2dict(args,kw_args,Nysol_Mnjoin.kwd))

