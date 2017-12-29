#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs
import nysol.take.mclique as mclique


args=margs.Margs(sys.argv,"ei=,ef=,ni=,nf=,-all,o=,l=,u=,log=","ei=,ef=")
mclique.mclique(args).run()

