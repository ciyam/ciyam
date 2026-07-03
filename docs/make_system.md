Understanding the Make System
-----------------------------

The makefile is generated from makefile.sio.xrep and so should never be changed directly (unless one does not
want to keep any changes that are made). So when adding/removing/renaming project files (excepting files that
belong to or are handled by the Meta application) changes need to be made manually to makefile.sio.xrep.

Once makefile.sio.xrep has been changed run ./genmake to reconstruct the actual makefile.

Generating the makefile with genmake
------------------------------------

The genmake bash script constructs the makefile with the use of xrep and xvars.

The xrep tool takes makefile.sio.xrep in order to generate makefile.sio which is in turn used as input by the
xvars tool in order to create makefile.vars.xrep.

Finally the xrep tool is used again with a specific makefile template (e.g. makefile.xrep for GCC) along with
the previously created makefile.vars.xrep to generate the project makefile.

Minimal make files for make system tools
----------------------------------------

As the tools xrep and xvars are required in order to construct the makefile a bootstrap problem arises in the
situation when a new repository clone is created and ./config is run for the very first time. To resolve this
issue a minimal "tools" make (e.g. makefile.tools for GCC) exists in the repository and will be used to build
these tools.
