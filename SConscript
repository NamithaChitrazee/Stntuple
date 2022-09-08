#!/usr/bin/env python

import os, re, string, subprocess, sys, importlib
Import('env')
sys.path.append(os.getenv("MUSE_WORK_DIR")+'/site_scons')
#------------------------------------------------------------------------------
# print("Stntuple/SConscript:muse branch: PWD:"+os.getenv("PWD"))

# pass the package name as a parameter

x = subprocess.call(os.getenv("MUSE_WORK_DIR")+'/Stntuple/scripts/build_config_muse Stntuple',shell=True)
# print("Stntuple/SConscript back from build_config_muse")

stntuple_env = env.Clone()
#------------------------------------------------------------------------------
# done
#------------------------------------------------------------------------------
exec(open(os.environ['MUSE_WORK_DIR']+"/site_scons/stntuple_site_init.py").read())

from stntuple_helper    import *

stntuple_env.Append(BUILDERS = {'StntupleCodegen'  : stntuple_codegen })
stntuple_env.Append(BUILDERS = {'StntupleRootCint' : stntuple_rootcint})

stntuple_env['CPPPATH' ].append(os.environ['MUSE_WORK_DIR']+'/include');

stntuple_env.Append(FORTRANPATH = [os.environ['MUSE_WORK_DIR']+'/include']);

# print(stntuple_env.Dump())

Export('stntuple_env')
Export('stntuple_helper')
