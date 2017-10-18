# -*- coding: utf-8 -*-
import nysol._nysolshell_core as n_core
from nysol.mod.nysollib.nysolmodcore import NysolMOD_CORE
from nysol.mod.nysollib import nysolutil as nutil

class Nysol_M2cross(NysolMOD_CORE):
	kwd = n_core.getparalist("m2cross")
	def __init__(self,*args, **kw_args) :
		super(Nysol_M2cross,self).__init__("m2cross",nutil.args2dict(args,kw_args,Nysol_M2cross.kwd))

