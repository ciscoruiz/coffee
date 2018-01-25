import os

usr_local_headers = ARGUMENTS.get('usr-local-header', '/usr/local/include')
usr_local_lib = ARGUMENTS.get('usr-local-lib', '/usr/local/lib')

user_cpp_compiler = ARGUMENTS.get('cpp-compiler', None)
user_cpp_path = ARGUMENTS.get('cpp-path', None)
user_lib_path = ARGUMENTS.get('lib-path', None)

target_usr_local_headers = os.path.join (usr_local_headers, "coffee")

release = ARGUMENTS.get ('release', 0)

env = Environment ()

current_directory = Dir ('.').abspath;

# CPPPATH will be relative to src/<target>
source_headers = os.path.join (current_directory, "include")

if user_cpp_compiler is not None:
   env['CXX'] = user_cpp_compiler

cpp_paths = [source_headers, usr_local_headers]

if user_cpp_path is not None:
   for path in user_cpp_path.split(','):
      cpp_paths.append(path)

env.Append (CPPPATH = cpp_paths)
env.Append (CCFLAGS = '-D__GXX_EXPERIMENTAL_CXX0X__ -std=c++11')
env.Append (LIBS = [''])

if int(release):
   env.Append (CCFLAGS = '-O3')
   env.Append (VARIANT = 'release')
else:
   env.Append (CCFLAGS = '-g -O0 -D_DEBUG')
   lib_paths = [os.path.join (current_directory, "debug")]

   if user_lib_path is not None:
      for path in user_lib_path.split(','):
         lib_paths.append(path)

   env.Append (LIBPATH = lib_paths)
   env.Append (VARIANT = 'debug')

libraries = [];

source = os.path.join (current_directory, "source")
sources = Glob(source + '/*')

sources=["adt", "config", "logger", "xml", "app", "balance", "dbms", "persistence"]

for module in sources:
   source = os.path.join (os.path.join(current_directory, 'source'), module)
   print source
   ss = str (source)
   ss += '/SConstruct'
   compile_library = SConscript (ss, exports='env')
   if (compile_library != None):
      libraries.extend (compile_library)

env.Default (libraries)

#
# Run 'scons test' to compile unit-tests
#
unit_tests = []
run_tests = []

source_headers = os.path.join (current_directory, "test")
env.Append (CPPPATH = [source_headers])

test = os.path.join (current_directory, "test")
tests = Glob(test + '/*')
for test in tests:
   ss = str (test)
   ss += '/SConstruct'
   test_unit_program = SConscript (ss, exports='env')   
   if test_unit_program != None:
      unit_tests.extend (test_unit_program)
      print test_unit_program [0]
      test_unit = Builder (action = '%s --report_level=short > $TARGET' % test_unit_program [0])
      env ['BUILDERS']['RunTestUnit'] = test_unit
      test_unit_result = env.RunTestUnit ('%s.output' % test_unit_program [0], 'SConstruct')   
      if test_unit_result != None:
         run_tests.extend (test_unit_result)
         Depends (test_unit_result, test_unit_program)
      

env.Alias ('test', [run_tests] )

env.Install (usr_local_lib, libraries)
env.Alias ("install-lib", usr_local_lib)


local_includes = os.path.join (current_directory, "include")
directory_headers = Glob (local_includes, "coffee")
env.Install (target_usr_local_headers, directory_headers)

env.Alias ("install-headers", target_usr_local_headers )

env.Alias ("install", ['install-lib', 'install-headers'])
 
env.Command ("uninstall", None, [ Delete (usr_local_lib), Delete (target_usr_local_headers) ])

