import os

prefix = ARGUMENTS.get ('prefix', "/usr/local")
usr_local_headers = os.path.join (prefix, "include")
usr_local_lib = os.path.join (prefix, "lib")

target_usr_local_headers = os.path.join (usr_local_headers, "hoost")

debug = ARGUMENTS.get ('debug', 0)

env = Environment ()

current_directory = Dir ('.').abspath;

# CPPPATH will be relative to src/<target>
source_headers = os.path.join (current_directory, "include")
env.Append (CPPPATH = [source_headers, usr_local_headers])
env.Append (LIBSUFFIX = '.a')
env.Append (LIBS = ['wepa'])

if int(debug):
   env.Append (CCFLAGS = '-g -O0')
   env.Append (LIBPATH = os.path.join (current_directory, "debug"))
   env.Append (VARIANT = 'debug')
else:
   env.Append (CCFLAGS = '-O3')
   env.Append (LIBPATH = os.path.join (current_directory, "release"))
   env.Append (VARIANT = 'release')

source = os.path.join (current_directory, "source")
sources = Glob(source + '/*')

for source in sources:
   ss = str (source)
   ss += '/SConstruct'
   compile_library = SConscript (ss, exports='env')
