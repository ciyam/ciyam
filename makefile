##
## Usage:
##  make [<target1> [<target2>] [...]]
##
##  where:
##  <target>   is:                  to:
##             all                  make all targets
##             dtm                  creates make.dtm
##             clean                clean all targets
##             usage                display this output
##
##             base                 make libbase.a target
##             commands             make libcommands.a target
##             cat_base             make cat_base.so target
##             dyn_lib              make dyn_lib.so target
##             Meta                 make Meta.so target
##             bundle               make bundle target
##             cat_client           make cat_client target
##             cat_interface        make cat_interface target
##             cat_server           make cat_server target
##             construct            make construct target
##             extract              make extract target
##             modeller             make modeller target
##             dump                 make dump target
##             demo                 make demo target
##             diff                 make diff target
##             diffp                make diffp target
##             menus                make menus target
##             crypt                make crypt target
##             btstore              make btstore target
##             client               make client target
##             server               make server target
##             streamer             make streamer target
##             streamer_client      make streamer_client target
##             streamer_server      make streamer_server target
##             date_time_test       make date_time_test target
##             popcheck             make popcheck target
##             sendmail             make sendmail target
##             generate_commands    make generate_commands target
##             sio_transform        make sio_transform target
##             time_client          make time_client target
##             time_server          make time_server target
##             test                 make test target
##             test_btree           make test_btree target
##             test_cache           make test_cache target
##             test_console         make test_console target
##             test_dyn_lib         make test_dyn_lib target
##             test_fcgi            make test_fcgi target
##             test_numeric         make test_numeric target
##             test_ods             make test_ods target
##             test_parser          make test_parser target
##             test_pdf_gen         make test_pdf_gen target
##             test_sql             make test_sql target
##             unbundle             make unbundle target
##             upload               make upload target
##             xrep                 make xrep target
##             xvars                make xvars target
##

# Tools
CPP = g++
BIND = ar
LINK = $(CPP)

# Compiler flags
CPPFLAGS = -ftemplate-depth-30 -felide-constructors -O3 -MMD -D__STDC_CONSTANT_MACROS -I/usr/local/ssl/include

# Implicit rules (and override for simple builtins)
%: %.o

%: %.cpp

%.o: %.cpp
	@./compile.sh $@ $< $(file_opts_$@)

%.cmh: %.cms
	@./generate_commands $<

%.compile:
#	@echo echo [$@] $(CPP) $(target_opts_$*) -c \$$3 $(CPPFLAGS) -o \$$1 \$$2 >compile.sh
	@echo echo \$$2 >compile.sh
	@echo $(CPP) $(target_opts_$*) -c \$$3 $(CPPFLAGS) -o \$$1 \$$2 >>compile.sh;chmod a+x compile.sh

# Notes: For file specific options use "file_opts_XXX.o = <opts>" (where XXX.cpp is the source file name)
# and for target specific options use "target_opts_XXX.so = <opts>" (where XXX.so is the name of the target).

BIND_NORMAL = @echo Binding $@...;$(BIND) -r $@

LINK_NORMAL = @echo Linking $@...;$(LINK) -o $@
LINK_NORMAL_DYN_USER = @echo Linking $@...;$(LINK) -ldl -o $@

LINK_DYNAMIC = @echo Linking $@...;$(LINK) -shared -lc -o $@

THREAD_LIBS = -lpthread
READLINE_LIBS = -lreadline
#READLINE_LIBS = -lreadline -ltermcap

OPENSSL_LIBS = -L/usr/local/ssl/lib -lssl -lcrypto -ldl

LIBFCGI_LIBS = -lfcgi

LIBHARU_LIBS = -lhpdf

LIBICONV_LIBS = -Liconv

MYSQL_LIBS = -lmysqlclient

ZLIB_LIBS = -lz

# Targets
TARGET_BASE = libbase.a
TARGET_COMMANDS = libcommands.a
TARGET_CAT_BASE = cat_base.so
TARGET_DYN_LIB = dyn_lib.so
TARGET_META = Meta.so
TARGET_BUNDLE = bundle
TARGET_CAT_CLIENT = cat_client
TARGET_CAT_INTERFACE = cat_interface
TARGET_CAT_SERVER = cat_server
TARGET_CONSTRUCT = construct
TARGET_EXTRACT = extract
TARGET_MODELLER = modeller
TARGET_DUMP = dump
TARGET_DEMO = demo
TARGET_DIFF = diff
TARGET_DIFFP = diffp
TARGET_MENUS = menus
TARGET_CRYPT = crypt
TARGET_BTSTORE = btstore
TARGET_CLIENT = client
TARGET_SERVER = server
TARGET_STREAMER = streamer
TARGET_STREAMER_CLIENT = streamer_client
TARGET_STREAMER_SERVER = streamer_server
TARGET_DATE_TIME_TEST = date_time_test
TARGET_POPCHECK = popcheck
TARGET_SENDMAIL = sendmail
TARGET_GENERATE_COMMANDS = generate_commands
TARGET_SIO_TRANSFORM = sio_transform
TARGET_TIME_CLIENT = time_client
TARGET_TIME_SERVER = time_server
TARGET_TEST = test
TARGET_TEST_BTREE = test_btree
TARGET_TEST_CACHE = test_cache
TARGET_TEST_CONSOLE = test_console
TARGET_TEST_DYN_LIB = test_dyn_lib
TARGET_TEST_FCGI = test_fcgi
TARGET_TEST_NUMERIC = test_numeric
TARGET_TEST_ODS = test_ods
TARGET_TEST_PARSER = test_parser
TARGET_TEST_PDF_GEN = test_pdf_gen
TARGET_TEST_SQL = test_sql
TARGET_UNBUNDLE = unbundle
TARGET_UPLOAD = upload
TARGET_XREP = xrep
TARGET_XVARS = xvars

# Source and object files
TARGET_BASE_CPPS = base64.cpp\
 console.cpp\
 console_commands.cpp\
 command_parser.cpp\
 command_handler.cpp\
 command_processor.cpp\
 crypt_stream.cpp\
 date_time.cpp\
 dynamic_library.cpp\
 file_buffer.cpp\
 format.cpp\
 fs_iterator.cpp\
 hashcash.cpp\
 mac.cpp\
 md5.cpp\
 mime.cpp\
 numeric.cpp\
 ods.cpp\
 pdf.cpp\
 pdf_gen.cpp\
 pop3.cpp\
 ptypes.cpp\
 read_write_buffer.cpp\
 read_write_buffered_stream.cpp\
 sio.cpp\
 sha1.cpp\
 smtp.cpp\
 sockets.cpp\
 ssl_socket.cpp\
 storable_file.cpp\
 sql_db.cpp\
 tcp_read_write_buffer.cpp\
 text_file_buffer.cpp\
 utilities.cpp
TARGET_BASE_OBJS = $(TARGET_BASE_CPPS:.cpp=.o)

TARGET_COMMANDS_CPPS = command_parser.cpp\
 utilities.cpp
TARGET_COMMANDS_OBJS = $(TARGET_COMMANDS_CPPS:.cpp=.o)

target_opts_cat_base.so = -fpic
TARGET_CAT_BASE_CPPS = cat_base.cpp\
 cat_base_command_handler.cpp\
 cat_base_date_time.cpp\
 cat_base_file_buffer.cpp\
 cat_base_numeric.cpp\
 cat_base_ods.cpp\
 class_base.cpp\
 commands_date_time.cpp\
 commands_numeric.cpp\
 mail_source.cpp\
 module_management.cpp
TARGET_CAT_BASE_OBJS = $(TARGET_CAT_BASE_CPPS:.cpp=.o)
TARGET_CAT_BASE_CMHS = date_time.cmh\
 numeric.cmh

target_opts_dyn_lib.so = -fpic
TARGET_DYN_LIB_CPPS = dyn_lib.cpp
TARGET_DYN_LIB_OBJS = $(TARGET_DYN_LIB_CPPS:.cpp=.o)

target_opts_Meta.so = -fpic
TARGET_META_CPPS = Meta.cpp\
 Meta_Application.cpp\
 Meta_Auto_Code.cpp\
 Meta_Class.cpp\
 Meta_Enum.cpp\
 Meta_Enum_Item.cpp\
 Meta_Field.cpp\
 Meta_Index.cpp\
 Meta_Initial_Record.cpp\
 Meta_Initial_Record_Value.cpp\
 Meta_List.cpp\
 Meta_List_Field.cpp\
 Meta_List_Field_Type.cpp\
 Meta_List_Type.cpp\
 Meta_Model.cpp\
 Meta_Modifier.cpp\
 Meta_Modifier_Affect.cpp\
 Meta_Module.cpp\
 Meta_Package.cpp\
 Meta_Package_Option.cpp\
 Meta_Package_Type.cpp\
 Meta_Permission.cpp\
 Meta_Procedure.cpp\
 Meta_Procedure_Arg.cpp\
 Meta_Relationship.cpp\
 Meta_Specification.cpp\
 Meta_Specification_Content_Page.cpp\
 Meta_Specification_Copy_Child_Links.cpp\
 Meta_Specification_Field_Action.cpp\
 Meta_Specification_Type.cpp\
 Meta_Type.cpp\
 Meta_User.cpp\
 Meta_View.cpp\
 Meta_View_Field.cpp\
 Meta_View_Field_Type.cpp\
 Meta_View_Type.cpp\
 Meta_Workgroup.cpp
TARGET_META_OBJS = $(TARGET_META_CPPS:.cpp=.o)
TARGET_META_CMHS = Meta.cmh\
 Meta_Application.cmh\
 Meta_Auto_Code.cmh\
 Meta_Class.cmh\
 Meta_Enum.cmh\
 Meta_Enum_Item.cmh\
 Meta_Field.cmh\
 Meta_Index.cmh\
 Meta_Initial_Record.cmh\
 Meta_Initial_Record_Value.cmh\
 Meta_List.cmh\
 Meta_List_Field.cmh\
 Meta_List_Field_Type.cmh\
 Meta_List_Type.cmh\
 Meta_Model.cmh\
 Meta_Modifier.cmh\
 Meta_Modifier_Affect.cmh\
 Meta_Module.cmh\
 Meta_Package.cmh\
 Meta_Package_Option.cmh\
 Meta_Package_Type.cmh\
 Meta_Permission.cmh\
 Meta_Procedure.cmh\
 Meta_Procedure_Arg.cmh\
 Meta_Relationship.cmh\
 Meta_Specification.cmh\
 Meta_Specification_Content_Page.cmh\
 Meta_Specification_Copy_Child_Links.cmh\
 Meta_Specification_Field_Action.cmh\
 Meta_Specification_Type.cmh\
 Meta_Type.cmh\
 Meta_User.cmh\
 Meta_View.cmh\
 Meta_View_Field.cmh\
 Meta_View_Field_Type.cmh\
 Meta_View_Type.cmh\
 Meta_Workgroup.cmh

TARGET_BUNDLE_CPPS = bundle.cpp
TARGET_BUNDLE_OBJS = $(TARGET_BUNDLE_CPPS:.cpp=.o)

TARGET_CAT_CLIENT_CPPS = cat_client.cpp
TARGET_CAT_CLIENT_OBJS = $(TARGET_CAT_CLIENT_CPPS:.cpp=.o)

TARGET_CAT_INTERFACE_CPPS = cat_interface.cpp\
 fcgi_cmds.cpp\
 fcgi_info.cpp\
 fcgi_list.cpp\
 fcgi_parser.cpp\
 fcgi_utils.cpp\
 fcgi_view.cpp
TARGET_CAT_INTERFACE_OBJS = $(TARGET_CAT_INTERFACE_CPPS:.cpp=.o)

TARGET_CAT_SERVER_CPPS = auto_script.cpp\
 cat_server.cpp\
 cat_session.cpp
TARGET_CAT_SERVER_OBJS = $(TARGET_CAT_SERVER_CPPS:.cpp=.o)
TARGET_CAT_SERVER_CMHS = cat_session.cmh

TARGET_CONSTRUCT_CPPS = construct.cpp
TARGET_CONSTRUCT_OBJS = $(TARGET_CONSTRUCT_CPPS:.cpp=.o)

TARGET_EXTRACT_CPPS = extract.cpp
TARGET_EXTRACT_OBJS = $(TARGET_EXTRACT_CPPS:.cpp=.o)

TARGET_MODELLER_CPPS = model.cpp\
 model_domains.cpp\
 model_specifications.cpp\
 interface_specifications.cpp\
 modeller.cpp
TARGET_MODELLER_OBJS = $(TARGET_MODELLER_CPPS:.cpp=.o)
TARGET_MODELLER_CMHS = modeller.cmh

TARGET_DUMP_CPPS = dump.cpp
TARGET_DUMP_OBJS = $(TARGET_DUMP_CPPS:.cpp=.o)

TARGET_DEMO_CPPS = demo.cpp
TARGET_DEMO_OBJS = $(TARGET_DEMO_CPPS:.cpp=.o)

TARGET_DIFF_CPPS = diff.cpp
TARGET_DIFF_OBJS = $(TARGET_DIFF_CPPS:.cpp=.o)

TARGET_DIFFP_CPPS = diffp.cpp
TARGET_DIFFP_OBJS = $(TARGET_DIFFP_CPPS:.cpp=.o)

TARGET_MENUS_CPPS = menus.cpp
TARGET_MENUS_OBJS = $(TARGET_MENUS_CPPS:.cpp=.o)

TARGET_CRYPT_CPPS = crypt.cpp
TARGET_CRYPT_OBJS = $(TARGET_CRYPT_CPPS:.cpp=.o)

TARGET_BTSTORE_CPPS = btstore.cpp
TARGET_BTSTORE_OBJS = $(TARGET_BTSTORE_CPPS:.cpp=.o)
TARGET_BTSTORE_CMHS = btstore.cmh

TARGET_CLIENT_CPPS = client.cpp
TARGET_CLIENT_OBJS = $(TARGET_CLIENT_CPPS:.cpp=.o)

TARGET_SERVER_CPPS = server.cpp\
 session.cpp
TARGET_SERVER_OBJS = $(TARGET_SERVER_CPPS:.cpp=.o)

TARGET_STREAMER_CPPS = streamer.cpp
TARGET_STREAMER_OBJS = $(TARGET_STREAMER_CPPS:.cpp=.o)

TARGET_STREAMER_CLIENT_CPPS = streamer_client.cpp
TARGET_STREAMER_CLIENT_OBJS = $(TARGET_STREAMER_CLIENT_CPPS:.cpp=.o)

TARGET_STREAMER_SERVER_CPPS = streamer_server.cpp
TARGET_STREAMER_SERVER_OBJS = $(TARGET_STREAMER_SERVER_CPPS:.cpp=.o)

TARGET_DATE_TIME_TEST_CPPS = date_time_test.cpp
TARGET_DATE_TIME_TEST_OBJS = $(TARGET_DATE_TIME_TEST_CPPS:.cpp=.o)

TARGET_POPCHECK_CPPS = popcheck.cpp
TARGET_POPCHECK_OBJS = $(TARGET_POPCHECK_CPPS:.cpp=.o)

TARGET_SENDMAIL_CPPS = sendmail.cpp
TARGET_SENDMAIL_OBJS = $(TARGET_SENDMAIL_CPPS:.cpp=.o)

TARGET_GENERATE_COMMANDS_CPPS = generate_commands.cpp
TARGET_GENERATE_COMMANDS_OBJS = $(TARGET_GENERATE_COMMANDS_CPPS:.cpp=.o)

TARGET_SIO_TRANSFORM_CPPS = sio_transform.cpp
TARGET_SIO_TRANSFORM_OBJS = $(TARGET_SIO_TRANSFORM_CPPS:.cpp=.o)

TARGET_TIME_CLIENT_CPPS = time_client.cpp
TARGET_TIME_CLIENT_OBJS = $(TARGET_TIME_CLIENT_CPPS:.cpp=.o)

TARGET_TIME_SERVER_CPPS = time_server.cpp
TARGET_TIME_SERVER_OBJS = $(TARGET_TIME_SERVER_CPPS:.cpp=.o)

TARGET_TEST_CPPS = test.cpp
TARGET_TEST_OBJS = $(TARGET_TEST_CPPS:.cpp=.o)

TARGET_TEST_BTREE_CPPS = test_btree.cpp
TARGET_TEST_BTREE_OBJS = $(TARGET_TEST_BTREE_CPPS:.cpp=.o)
TARGET_TEST_BTREE_CMHS = test_btree.cmh

TARGET_TEST_CACHE_CPPS = test_cache.cpp
TARGET_TEST_CACHE_OBJS = $(TARGET_TEST_CACHE_CPPS:.cpp=.o)
TARGET_TEST_CACHE_CMHS = test_cache.cmh

TARGET_TEST_CONSOLE_CPPS = test_console.cpp
TARGET_TEST_CONSOLE_OBJS = $(TARGET_TEST_CONSOLE_CPPS:.cpp=.o)
TARGET_TEST_CONSOLE_CMHS = test_console.cmh

TARGET_TEST_DYN_LIB_CPPS = test_dyn_lib.cpp
TARGET_TEST_DYN_LIB_OBJS = $(TARGET_TEST_DYN_LIB_CPPS:.cpp=.o)

TARGET_TEST_FCGI_CPPS = test_fcgi.cpp
TARGET_TEST_FCGI_OBJS = $(TARGET_TEST_FCGI_CPPS:.cpp=.o)

TARGET_TEST_NUMERIC_CPPS = test_numeric.cpp
TARGET_TEST_NUMERIC_OBJS = $(TARGET_TEST_NUMERIC_CPPS:.cpp=.o)
TARGET_TEST_NUMERIC_CMHS = test_numeric.cmh

TARGET_TEST_ODS_CPPS = test_ods.cpp
TARGET_TEST_ODS_OBJS = $(TARGET_TEST_ODS_CPPS:.cpp=.o)
TARGET_TEST_ODS_CMHS = test_ods.cmh

TARGET_TEST_PARSER_CPPS = test_parser.cpp
TARGET_TEST_PARSER_OBJS = $(TARGET_TEST_PARSER_CPPS:.cpp=.o)

TARGET_TEST_PDF_GEN_CPPS = test_pdf_gen.cpp
TARGET_TEST_PDF_GEN_OBJS = $(TARGET_TEST_PDF_GEN_CPPS:.cpp=.o)
TARGET_TEST_PDF_GEN_CMHS = test_pdf_gen.cmh

TARGET_TEST_SQL_CPPS = test_sql.cpp
TARGET_TEST_SQL_OBJS = $(TARGET_TEST_SQL_CPPS:.cpp=.o)

TARGET_UNBUNDLE_CPPS = unbundle.cpp
TARGET_UNBUNDLE_OBJS = $(TARGET_UNBUNDLE_CPPS:.cpp=.o)

TARGET_UPLOAD_CPPS = upload.cpp
TARGET_UPLOAD_OBJS = $(TARGET_UPLOAD_CPPS:.cpp=.o)

TARGET_XREP_CPPS = xrep.cpp
TARGET_XREP_OBJS = $(TARGET_XREP_CPPS:.cpp=.o)

TARGET_XVARS_CPPS = xvars.cpp
TARGET_XVARS_OBJS = $(TARGET_XVARS_CPPS:.cpp=.o)

ALL_TARGETS =\
 $(TARGET_BASE)\
 $(TARGET_COMMANDS)\
 $(TARGET_CAT_BASE)\
 $(TARGET_DYN_LIB)\
 $(TARGET_META)\
 $(TARGET_BUNDLE)\
 $(TARGET_CAT_CLIENT)\
 $(TARGET_CAT_INTERFACE)\
 $(TARGET_CAT_SERVER)\
 $(TARGET_CONSTRUCT)\
 $(TARGET_EXTRACT)\
 $(TARGET_MODELLER)\
 $(TARGET_DUMP)\
 $(TARGET_DEMO)\
 $(TARGET_DIFF)\
 $(TARGET_DIFFP)\
 $(TARGET_MENUS)\
 $(TARGET_CRYPT)\
 $(TARGET_BTSTORE)\
 $(TARGET_CLIENT)\
 $(TARGET_SERVER)\
 $(TARGET_STREAMER)\
 $(TARGET_STREAMER_CLIENT)\
 $(TARGET_STREAMER_SERVER)\
 $(TARGET_DATE_TIME_TEST)\
 $(TARGET_POPCHECK)\
 $(TARGET_SENDMAIL)\
 $(TARGET_GENERATE_COMMANDS)\
 $(TARGET_SIO_TRANSFORM)\
 $(TARGET_TIME_CLIENT)\
 $(TARGET_TIME_SERVER)\
 $(TARGET_TEST)\
 $(TARGET_TEST_BTREE)\
 $(TARGET_TEST_CACHE)\
 $(TARGET_TEST_CONSOLE)\
 $(TARGET_TEST_DYN_LIB)\
 $(TARGET_TEST_FCGI)\
 $(TARGET_TEST_NUMERIC)\
 $(TARGET_TEST_ODS)\
 $(TARGET_TEST_PARSER)\
 $(TARGET_TEST_PDF_GEN)\
 $(TARGET_TEST_SQL)\
 $(TARGET_UNBUNDLE)\
 $(TARGET_UPLOAD)\
 $(TARGET_XREP)\
 $(TARGET_XVARS)

ALL_CPP_FILES =\
 $(TARGET_BASE_CPPS)\
 $(TARGET_COMMANDS_CPPS)\
 $(TARGET_CAT_BASE_CPPS)\
 $(TARGET_DYN_LIB_CPPS)\
 $(TARGET_META_CPPS)\
 $(TARGET_BUNDLE_CPPS)\
 $(TARGET_CAT_CLIENT_CPPS)\
 $(TARGET_CAT_INTERFACE_CPPS)\
 $(TARGET_CAT_SERVER_CPPS)\
 $(TARGET_CONSTRUCT_CPPS)\
 $(TARGET_EXTRACT_CPPS)\
 $(TARGET_MODELLER_CPPS)\
 $(TARGET_DUMP_CPPS)\
 $(TARGET_DEMO_CPPS)\
 $(TARGET_DIFF_CPPS)\
 $(TARGET_DIFFP_CPPS)\
 $(TARGET_MENUS_CPPS)\
 $(TARGET_CRYPT_CPPS)\
 $(TARGET_BTSTORE_CPPS)\
 $(TARGET_CLIENT_CPPS)\
 $(TARGET_SERVER_CPPS)\
 $(TARGET_STREAMER_CPPS)\
 $(TARGET_STREAMER_CLIENT_CPPS)\
 $(TARGET_STREAMER_SERVER_CPPS)\
 $(TARGET_DATE_TIME_TEST_CPPS)\
 $(TARGET_POPCHECK_CPPS)\
 $(TARGET_SENDMAIL_CPPS)\
 $(TARGET_GENERATE_COMMANDS_CPPS)\
 $(TARGET_SIO_TRANSFORM_CPPS)\
 $(TARGET_TIME_CLIENT_CPPS)\
 $(TARGET_TIME_SERVER_CPPS)\
 $(TARGET_TEST_CPPS)\
 $(TARGET_TEST_BTREE_CPPS)\
 $(TARGET_TEST_CACHE_CPPS)\
 $(TARGET_TEST_CONSOLE_CPPS)\
 $(TARGET_TEST_DYN_LIB_CPPS)\
 $(TARGET_TEST_FCGI_CPPS)\
 $(TARGET_TEST_NUMERIC_CPPS)\
 $(TARGET_TEST_ODS_CPPS)\
 $(TARGET_TEST_PARSER_CPPS)\
 $(TARGET_TEST_PDF_GEN_CPPS)\
 $(TARGET_TEST_SQL_CPPS)\
 $(TARGET_UNBUNDLE_CPPS)\
 $(TARGET_UPLOAD_CPPS)\
 $(TARGET_XREP_CPPS)\
 $(TARGET_XVARS_CPPS)

ALL_OBJ_FILES =\
 $(TARGET_BASE_OBJS)\
 $(TARGET_COMMANDS_OBJS)\
 $(TARGET_CAT_BASE_OBJS)\
 $(TARGET_DYN_LIB_OBJS)\
 $(TARGET_META_OBJS)\
 $(TARGET_BUNDLE_OBJS)\
 $(TARGET_CAT_CLIENT_OBJS)\
 $(TARGET_CAT_INTERFACE_OBJS)\
 $(TARGET_CAT_SERVER_OBJS)\
 $(TARGET_CONSTRUCT_OBJS)\
 $(TARGET_EXTRACT_OBJS)\
 $(TARGET_MODELLER_OBJS)\
 $(TARGET_DUMP_OBJS)\
 $(TARGET_DEMO_OBJS)\
 $(TARGET_DIFF_OBJS)\
 $(TARGET_DIFFP_OBJS)\
 $(TARGET_MENUS_OBJS)\
 $(TARGET_CRYPT_OBJS)\
 $(TARGET_BTSTORE_OBJS)\
 $(TARGET_CLIENT_OBJS)\
 $(TARGET_SERVER_OBJS)\
 $(TARGET_STREAMER_OBJS)\
 $(TARGET_STREAMER_CLIENT_OBJS)\
 $(TARGET_STREAMER_SERVER_OBJS)\
 $(TARGET_DATE_TIME_TEST_OBJS)\
 $(TARGET_POPCHECK_OBJS)\
 $(TARGET_SENDMAIL_OBJS)\
 $(TARGET_GENERATE_COMMANDS_OBJS)\
 $(TARGET_SIO_TRANSFORM_OBJS)\
 $(TARGET_TIME_CLIENT_OBJS)\
 $(TARGET_TIME_SERVER_OBJS)\
 $(TARGET_TEST_OBJS)\
 $(TARGET_TEST_BTREE_OBJS)\
 $(TARGET_TEST_CACHE_OBJS)\
 $(TARGET_TEST_CONSOLE_OBJS)\
 $(TARGET_TEST_DYN_LIB_OBJS)\
 $(TARGET_TEST_FCGI_OBJS)\
 $(TARGET_TEST_NUMERIC_OBJS)\
 $(TARGET_TEST_ODS_OBJS)\
 $(TARGET_TEST_PARSER_OBJS)\
 $(TARGET_TEST_PDF_GEN_OBJS)\
 $(TARGET_TEST_SQL_OBJS)\
 $(TARGET_UNBUNDLE_OBJS)\
 $(TARGET_UPLOAD_OBJS)\
 $(TARGET_XREP_OBJS)\
 $(TARGET_XVARS_OBJS)

ALL_CMH_FILES =\
 $(TARGET_CAT_BASE_CMHS)\
 $(TARGET_META_CMHS)\
 $(TARGET_CAT_SERVER_CMHS)\
 $(TARGET_MODELLER_CMHS)\
 $(TARGET_BTSTORE_CMHS)\
 $(TARGET_TEST_BTREE_CMHS)\
 $(TARGET_TEST_CACHE_CMHS)\
 $(TARGET_TEST_CONSOLE_CMHS)\
 $(TARGET_TEST_NUMERIC_CMHS)\
 $(TARGET_TEST_ODS_CMHS)\
 $(TARGET_TEST_PDF_GEN_CMHS)

ALL_DEP_FILES = $(ALL_CPP_FILES:.cpp=.d)

# Targets
usage: make_usage

base: $(TARGET_BASE)

commands: $(TARGET_COMMANDS)

cat_base: $(TARGET_CAT_BASE)

dyn_lib: $(TARGET_DYN_LIB)

Meta: $(TARGET_META)

all:\
 $(TARGET_BASE)\
 $(TARGET_COMMANDS)\
 $(TARGET_CAT_BASE)\
 $(TARGET_DYN_LIB)\
 $(TARGET_META)\
 $(TARGET_BUNDLE)\
 $(TARGET_CAT_CLIENT)\
 $(TARGET_CAT_INTERFACE)\
 $(TARGET_CAT_SERVER)\
 $(TARGET_CONSTRUCT)\
 $(TARGET_EXTRACT)\
 $(TARGET_MODELLER)\
 $(TARGET_DUMP)\
 $(TARGET_DEMO)\
 $(TARGET_DIFF)\
 $(TARGET_DIFFP)\
 $(TARGET_MENUS)\
 $(TARGET_CRYPT)\
 $(TARGET_BTSTORE)\
 $(TARGET_CLIENT)\
 $(TARGET_SERVER)\
 $(TARGET_STREAMER)\
 $(TARGET_STREAMER_CLIENT)\
 $(TARGET_STREAMER_SERVER)\
 $(TARGET_DATE_TIME_TEST)\
 $(TARGET_POPCHECK)\
 $(TARGET_SENDMAIL)\
 $(TARGET_GENERATE_COMMANDS)\
 $(TARGET_SIO_TRANSFORM)\
 $(TARGET_TIME_CLIENT)\
 $(TARGET_TIME_SERVER)\
 $(TARGET_TEST)\
 $(TARGET_TEST_BTREE)\
 $(TARGET_TEST_CACHE)\
 $(TARGET_TEST_CONSOLE)\
 $(TARGET_TEST_DYN_LIB)\
 $(TARGET_TEST_FCGI)\
 $(TARGET_TEST_NUMERIC)\
 $(TARGET_TEST_ODS)\
 $(TARGET_TEST_PARSER)\
 $(TARGET_TEST_PDF_GEN)\
 $(TARGET_TEST_SQL)\
 $(TARGET_UNBUNDLE)\
 $(TARGET_UPLOAD)\
 $(TARGET_XREP)\
 $(TARGET_XVARS)

dtm:
	@echo `date`>make.dtm

make_usage:
	@grep ^## makefile

$(TARGET_BASE):: $(TARGET_BASE).compile

$(TARGET_BASE):: $(TARGET_BASE_OBJS)
	$(BIND_NORMAL) $(TARGET_BASE_OBJS)

$(TARGET_COMMANDS):: $(TARGET_COMMANDS).compile

$(TARGET_COMMANDS):: $(TARGET_COMMANDS_OBJS)
	$(BIND_NORMAL) $(TARGET_COMMANDS_OBJS)

$(TARGET_CAT_BASE):: $(TARGET_BASE) $(TARGET_GENERATE_COMMANDS) $(TARGET_CAT_BASE_CMHS) $(TARGET_CAT_BASE).compile

$(TARGET_CAT_BASE):: $(TARGET_CAT_BASE_OBJS) $(TARGET_BASE)
	$(LINK_DYNAMIC) $(TARGET_CAT_BASE_OBJS) $(READLINE_LIBS) $(LIBHARU_LIBS) $(LIBICONV_LIBS) $(OPENSSL_LIBS) $(MYSQL_LIBS) -L. -lbase

$(TARGET_DYN_LIB):: $(TARGET_DYN_LIB).compile

$(TARGET_DYN_LIB):: $(TARGET_DYN_LIB_OBJS)
	$(LINK_DYNAMIC) $(TARGET_DYN_LIB_OBJS)

$(TARGET_META):: $(TARGET_CAT_BASE) $(TARGET_GENERATE_COMMANDS) $(TARGET_META_CMHS) $(TARGET_META).compile

$(TARGET_META):: $(TARGET_META_OBJS) $(TARGET_CAT_BASE)
	$(LINK_DYNAMIC) $(TARGET_META_OBJS) -ldl -Wl,-rpath,. $(TARGET_CAT_BASE)

$(TARGET_BUNDLE):: $(TARGET_BASE) $(TARGET_BUNDLE).compile

$(TARGET_BUNDLE):: $(TARGET_BUNDLE_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_BUNDLE_OBJS) $(ZLIB_LIBS) -L. -lbase

$(TARGET_CAT_CLIENT):: $(TARGET_BASE) $(TARGET_CAT_CLIENT).compile

$(TARGET_CAT_CLIENT):: $(TARGET_CAT_CLIENT_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_CAT_CLIENT_OBJS) $(READLINE_LIBS) -L. -lbase

$(TARGET_CAT_INTERFACE):: $(TARGET_BASE) $(TARGET_CAT_INTERFACE).compile

$(TARGET_CAT_INTERFACE):: $(TARGET_CAT_INTERFACE_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_CAT_INTERFACE_OBJS) $(THREAD_LIBS) $(READLINE_LIBS) $(LIBFCGI_LIBS) -L. -lbase

$(TARGET_CAT_SERVER):: $(TARGET_BASE) $(TARGET_CAT_BASE) $(TARGET_GENERATE_COMMANDS) $(TARGET_CAT_SERVER_CMHS) $(TARGET_CAT_SERVER).compile

$(TARGET_CAT_SERVER):: $(TARGET_CAT_SERVER_OBJS) $(TARGET_BASE) $(TARGET_CAT_BASE)
	$(LINK_NORMAL) $(TARGET_CAT_SERVER_OBJS) $(THREAD_LIBS) $(LIBHARU_LIBS) $(LIBICONV_LIBS) $(OPENSSL_LIBS) -L. -lbase -ldl -Wl,-rpath,. $(TARGET_CAT_BASE)

$(TARGET_CONSTRUCT):: $(TARGET_BASE) $(TARGET_CONSTRUCT).compile

$(TARGET_CONSTRUCT):: $(TARGET_CONSTRUCT_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_CONSTRUCT_OBJS) -L. -lbase

$(TARGET_EXTRACT):: $(TARGET_EXTRACT).compile

$(TARGET_EXTRACT):: $(TARGET_EXTRACT_OBJS)
	$(LINK_NORMAL) $(TARGET_EXTRACT_OBJS)

$(TARGET_MODELLER):: $(TARGET_BASE) $(TARGET_GENERATE_COMMANDS) $(TARGET_MODELLER_CMHS) $(TARGET_MODELLER).compile

$(TARGET_MODELLER):: $(TARGET_MODELLER_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_MODELLER_OBJS) $(READLINE_LIBS) -L. -lbase

$(TARGET_DUMP):: $(TARGET_BASE) $(TARGET_DUMP).compile

$(TARGET_DUMP):: $(TARGET_DUMP_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_DUMP_OBJS) -L. -lbase

$(TARGET_DEMO):: $(TARGET_BASE) $(TARGET_DEMO).compile

$(TARGET_DEMO):: $(TARGET_DEMO_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_DEMO_OBJS) $(THREAD_LIBS) -L. -lbase

$(TARGET_DIFF):: $(TARGET_BASE) $(TARGET_DIFF).compile

$(TARGET_DIFF):: $(TARGET_DIFF_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_DIFF_OBJS) -L. -lbase

$(TARGET_DIFFP):: $(TARGET_BASE) $(TARGET_DIFFP).compile

$(TARGET_DIFFP):: $(TARGET_DIFFP_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_DIFFP_OBJS) -L. -lbase

$(TARGET_MENUS):: $(TARGET_BASE) $(TARGET_MENUS).compile

$(TARGET_MENUS):: $(TARGET_MENUS_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_MENUS_OBJS) $(READLINE_LIBS) -L. -lbase

$(TARGET_CRYPT):: $(TARGET_BASE) $(TARGET_CRYPT).compile

$(TARGET_CRYPT):: $(TARGET_CRYPT_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_CRYPT_OBJS) $(READLINE_LIBS) -L. -lbase

$(TARGET_BTSTORE):: $(TARGET_BASE) $(TARGET_GENERATE_COMMANDS) $(TARGET_BTSTORE_CMHS) $(TARGET_BTSTORE).compile

$(TARGET_BTSTORE):: $(TARGET_BTSTORE_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_BTSTORE_OBJS) $(READLINE_LIBS) -L. -lbase

$(TARGET_CLIENT):: $(TARGET_BASE) $(TARGET_CLIENT).compile

$(TARGET_CLIENT):: $(TARGET_CLIENT_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_CLIENT_OBJS) -L. -lbase

$(TARGET_SERVER):: $(TARGET_BASE) $(TARGET_SERVER).compile

$(TARGET_SERVER):: $(TARGET_SERVER_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_SERVER_OBJS) $(THREAD_LIBS) -L. -lbase

$(TARGET_STREAMER):: $(TARGET_STREAMER).compile

$(TARGET_STREAMER):: $(TARGET_STREAMER_OBJS)
	$(LINK_NORMAL) $(TARGET_STREAMER_OBJS)

$(TARGET_STREAMER_CLIENT):: $(TARGET_BASE) $(TARGET_STREAMER_CLIENT).compile

$(TARGET_STREAMER_CLIENT):: $(TARGET_STREAMER_CLIENT_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_STREAMER_CLIENT_OBJS) -L. -lbase

$(TARGET_STREAMER_SERVER):: $(TARGET_BASE) $(TARGET_STREAMER_SERVER).compile

$(TARGET_STREAMER_SERVER):: $(TARGET_STREAMER_SERVER_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_STREAMER_SERVER_OBJS) -L. -lbase

$(TARGET_DATE_TIME_TEST):: $(TARGET_BASE) $(TARGET_DATE_TIME_TEST).compile

$(TARGET_DATE_TIME_TEST):: $(TARGET_DATE_TIME_TEST_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_DATE_TIME_TEST_OBJS) -L. -lbase

$(TARGET_POPCHECK):: $(TARGET_BASE) $(TARGET_POPCHECK).compile

$(TARGET_POPCHECK):: $(TARGET_POPCHECK_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_POPCHECK_OBJS) $(READLINE_LIBS) $(OPENSSL_LIBS) -L. -lbase

$(TARGET_SENDMAIL):: $(TARGET_BASE) $(TARGET_SENDMAIL).compile

$(TARGET_SENDMAIL):: $(TARGET_SENDMAIL_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_SENDMAIL_OBJS) $(READLINE_LIBS) $(OPENSSL_LIBS) -L. -lbase

$(TARGET_GENERATE_COMMANDS):: $(TARGET_COMMANDS) $(TARGET_GENERATE_COMMANDS).compile

$(TARGET_GENERATE_COMMANDS):: $(TARGET_GENERATE_COMMANDS_OBJS) $(TARGET_COMMANDS)
	$(LINK_NORMAL) $(TARGET_GENERATE_COMMANDS_OBJS) -L. -lcommands
	@rm -f *.cmh

$(TARGET_SIO_TRANSFORM):: $(TARGET_BASE) $(TARGET_SIO_TRANSFORM).compile

$(TARGET_SIO_TRANSFORM):: $(TARGET_SIO_TRANSFORM_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_SIO_TRANSFORM_OBJS) -L. -lbase

$(TARGET_TIME_CLIENT):: $(TARGET_BASE) $(TARGET_TIME_CLIENT).compile

$(TARGET_TIME_CLIENT):: $(TARGET_TIME_CLIENT_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_TIME_CLIENT_OBJS) -L. -lbase

$(TARGET_TIME_SERVER):: $(TARGET_BASE) $(TARGET_TIME_SERVER).compile

$(TARGET_TIME_SERVER):: $(TARGET_TIME_SERVER_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_TIME_SERVER_OBJS) -L. -lbase

$(TARGET_TEST):: $(TARGET_BASE) $(TARGET_TEST).compile

$(TARGET_TEST):: $(TARGET_TEST_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_TEST_OBJS) -L. -lbase

$(TARGET_TEST_BTREE):: $(TARGET_BASE) $(TARGET_GENERATE_COMMANDS) $(TARGET_TEST_BTREE_CMHS) $(TARGET_TEST_BTREE).compile

$(TARGET_TEST_BTREE):: $(TARGET_TEST_BTREE_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_TEST_BTREE_OBJS) $(READLINE_LIBS) -L. -lbase

$(TARGET_TEST_CACHE):: $(TARGET_BASE) $(TARGET_GENERATE_COMMANDS) $(TARGET_TEST_CACHE_CMHS) $(TARGET_TEST_CACHE).compile

$(TARGET_TEST_CACHE):: $(TARGET_TEST_CACHE_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_TEST_CACHE_OBJS) $(READLINE_LIBS) -L. -lbase

$(TARGET_TEST_CONSOLE):: $(TARGET_BASE) $(TARGET_GENERATE_COMMANDS) $(TARGET_TEST_CONSOLE_CMHS) $(TARGET_TEST_CONSOLE).compile

$(TARGET_TEST_CONSOLE):: $(TARGET_TEST_CONSOLE_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_TEST_CONSOLE_OBJS) $(READLINE_LIBS) -L. -lbase

$(TARGET_TEST_DYN_LIB):: $(TARGET_BASE) $(TARGET_TEST_DYN_LIB).compile

$(TARGET_TEST_DYN_LIB):: $(TARGET_TEST_DYN_LIB_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL_DYN_USER) $(TARGET_TEST_DYN_LIB_OBJS) -L. -lbase

$(TARGET_TEST_FCGI):: $(TARGET_TEST_FCGI).compile

$(TARGET_TEST_FCGI):: $(TARGET_TEST_FCGI_OBJS)
	$(LINK_NORMAL_DYN_USER) $(TARGET_TEST_FCGI_OBJS) $(LIBFCGI_LIBS)

$(TARGET_TEST_NUMERIC):: $(TARGET_BASE) $(TARGET_GENERATE_COMMANDS) $(TARGET_TEST_NUMERIC_CMHS) $(TARGET_TEST_NUMERIC).compile

$(TARGET_TEST_NUMERIC):: $(TARGET_TEST_NUMERIC_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_TEST_NUMERIC_OBJS) $(READLINE_LIBS) -L. -lbase

$(TARGET_TEST_ODS):: $(TARGET_BASE) $(TARGET_GENERATE_COMMANDS) $(TARGET_TEST_ODS_CMHS) $(TARGET_TEST_ODS).compile

$(TARGET_TEST_ODS):: $(TARGET_TEST_ODS_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_TEST_ODS_OBJS) $(READLINE_LIBS) -L. -lbase

$(TARGET_TEST_PARSER):: $(TARGET_BASE) $(TARGET_TEST_PARSER).compile

$(TARGET_TEST_PARSER):: $(TARGET_TEST_PARSER_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_TEST_PARSER_OBJS) -L. -lbase

$(TARGET_TEST_PDF_GEN):: $(TARGET_BASE) $(TARGET_GENERATE_COMMANDS) $(TARGET_TEST_PDF_GEN_CMHS) $(TARGET_TEST_PDF_GEN).compile

$(TARGET_TEST_PDF_GEN):: $(TARGET_TEST_PDF_GEN_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_TEST_PDF_GEN_OBJS) $(READLINE_LIBS) $(LIBHARU_LIBS) $(LIBICONV_LIBS) -L. -lbase

$(TARGET_TEST_SQL):: $(TARGET_BASE) $(TARGET_TEST_SQL).compile

$(TARGET_TEST_SQL):: $(TARGET_TEST_SQL_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_TEST_SQL_OBJS) $(THREAD_LIBS) $(MYSQL_LIBS) -L. -lbase

$(TARGET_UNBUNDLE):: $(TARGET_BASE) $(TARGET_UNBUNDLE).compile

$(TARGET_UNBUNDLE):: $(TARGET_UNBUNDLE_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_UNBUNDLE_OBJS) $(READLINE_LIBS) $(ZLIB_LIBS) -L. -lbase

$(TARGET_UPLOAD):: $(TARGET_BASE) $(TARGET_UPLOAD).compile

$(TARGET_UPLOAD):: $(TARGET_UPLOAD_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_UPLOAD_OBJS) $(THREAD_LIBS) $(LIBFCGI_LIBS) -L. -lbase

$(TARGET_XREP):: $(TARGET_BASE) $(TARGET_XREP).compile

$(TARGET_XREP):: $(TARGET_XREP_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_XREP_OBJS) -L. -lbase

$(TARGET_XVARS):: $(TARGET_BASE) $(TARGET_XVARS).compile

$(TARGET_XVARS):: $(TARGET_XVARS_OBJS) $(TARGET_BASE)
	$(LINK_NORMAL) $(TARGET_XVARS_OBJS) -L. -lbase

clean:
	@rm -f compile.sh $(ALL_CMH_FILES) $(ALL_OBJ_FILES) $(ALL_DEP_FILES) $(ALL_TARGETS)

# Dependency include
-include $(ALL_DEP_FILES)

