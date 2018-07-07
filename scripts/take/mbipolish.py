#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs
import nysol.take as nt

args=margs.Margs(sys.argv,"ei=,ef=,eo=,th=,sim=,th2=,sim2=,kn=,kn2=,sup=,iter=,log=","ei=,ef=,th=")
nt.mbipolish(**args.kvmap()).run()



