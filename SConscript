#!/usr/bin/env python

import os, re, string, subprocess, sys, importlib
Import('env')
sys.path.append(os.getenv("MUSE_WORK_DIR")+'/site_scons')

# from SCons.Script import *
#------------------------------------------------------------------------------
# last two components of the path. Ex: /not/this/but/THIS/AND_THIS
#                                      "AND_THIS" is usually "src"
#------------------------------------------------------------------------------
print("Stntuple/SConscript:emoe: PWD:"+os.getenv("PWD"))

x = subprocess.call(os.getenv("MUSE_WORK_DIR")+'/Stntuple/scripts/build_config_muse',shell=True)

stntuple_env = env.Clone()

stntuple_env['CPPPATH' ].append('-I'+os.environ['MUSE_WORK_DIR']+'/build/include');
stntuple_env['CXXFLAGS'].append('-I'+os.environ['MUSE_WORK_DIR']+'/build/include');
stntuple_env.Append(FORTRANFLAGS = ['-I'+os.environ['MUSE_WORK_DIR']+'/build/include']);
#------------------------------------------------------------------------------
# done
#------------------------------------------------------------------------------
exec(open(os.environ['MUSE_WORK_DIR']+"/site_scons/stntuple_site_init.py").read())

from stntuple_helper    import *

stntuple_env.Append(BUILDERS = {'StntupleCodegen'  : stntuple_codegen })
stntuple_env.Append(BUILDERS = {'StntupleRootCint' : stntuple_rootcint})

Export('stntuple_env')
Export('stntuple_helper')
