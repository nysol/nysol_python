#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs
import nysol.take as nt


args=margs.Margs(sys.argv,"ei=,ef=,eI=,eF=,ni=,nf=,nI=,nF=,-dir,eo=,no=,T=","ei=,eI=,eo=")
nt.mgdiff(**args.kvmap()).run(msg="on")

