 ----------  Expert-N 5.0  -----------------------------------

Expert-N uses the waf meta build system. Documentation, IDE Support, etc can be found at:
http://code.google.com/p/waf/

-----------  Requirements -----------------------------------

 --- waf: --------------
python 2.x:
Windows: python 2.7 from http://www.python.org/
Linux: Should be already installed

 ----- Compiler: -----
Windows: MinGW (http://www.mingw.org/) or MS Visual C++ 
Linux: gcc/g++: Packet Manager

 ----- Tools: --------
pkg-config
Windows: Use the all-in-one bundle on http://www.gtk.org/
Linux: Packet Manager

 ----- Libraries -----
 
glib-2.0, gobject-2.0, gmodule-2.0:
Windows: Use the all-in-one bundle on http://www.gtk.org/ http://www.gtk.org/
Linux: Development Packets of your Distribution

libgda-4.0:
Windows: http://www.gnome-db.org/
Linux: Development Packets of your Distribution

netcdf 4.2:
Windows: ftp://ftp.unidata.ucar.edu/pub/netcdf/
Linux: ftp://ftp.unidata.ucar.edu/pub/netcdf/
see also http://code.google.com/p/netcdf4-python/w/list



----------- Build instructions ------------------------------

To compile Expert-N, three steps are necessary:

./waf configure
./waf build
./waf install


Some setup options:

Installation Path:
./waf configure --prefix=/home/chrikle/expertn50/built

Debugging:
./waf configure --debug

Full Speed Optimization 
./waf configure --max_optimize

Compile with OpenMP Parallelization:
./waf configure --USE_OPENMP

Compile with MPI Parallelization:
./waf configure --USE_MPI

compile a special module:
./waf list (show all projects)
./waf build --target=module (compiles one project)

e.g.:
./waf build --target=expertn_modul_base
or
./waf build --target=expertn_modul_base,expertn_database



--- Create Codelite, Visual Studio or Eclipse Project  ---------------
./waf configure codelite
or
./waf configure msvs2008
or (for Visual Studio 20010)
./waf configure msvs
or (for Eclipse)
./ waf configure eclipse


----------- General Options of the waf build system --------

Main commands:
build    : executes the build
clean    : cleans the project
configure: configures the project
dist     : makes a tarball for redistributing the sources
distcheck: checks if the project compiles (tarball from 'dist')
distclean: removes the build directory
install  : installs the targets on the system
list     : lists the targets to execute
step     : executes tasks in a step-by-step fashion, for debugging
uninstall: removes the targets installed

Options:
  --version             show program's version number and exit
  -h, --help            show this help message and exit
  -j JOBS, --jobs=JOBS  amount of parallel jobs (2)
  -k, --keep            keep running happily even if errors are found
  -v, --verbose         verbosity level -v -vv or -vvv [default: 0]
  --nocache             ignore the WAFCACHE (if set)
  --zones=ZONES         debugging zones (task_gen, deps, tasks, etc)

  configure options:
    -o OUT, --out=OUT   build dir for the project
    -t TOP, --top=TOP   src dir for the project
    --prefix=PREFIX     installation prefix [default: '/usr/local/']
    --download          try to download the tools if missing


