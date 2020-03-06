#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import nysol.util.margs as margs
import nysol.take as nt


args=margs.Margs(sys.argv,"ei=,ef=,ni=,nf=,o=,-verbose","ei=,ef=")
nt.mccomp(**(args.kvmap())).run(msg="on")

