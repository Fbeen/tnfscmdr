#
# TNFSCMDR makefile
#

tcp_h_dir = ../mTCP/tcpinc
tcp_c_dir = ../mTCP/tcplib

src_dir = src
inc_dir = include
build_dir = build

application = tnfscmdr

target = dos
memory_model = -ms

error_file = $(build_dir)/$(application).err

compile_options = -bt=$(target) -0 $(memory_model)
compile_options += -d+
compile_options += -DCFG_H='"$(application).cfg"'
compile_options += -oh -ok -os -s -oa -ei -zp2 -zpw
compile_options += -i=$(tcp_h_dir)
compile_options += -i=$(inc_dir)
compile_options += -i=$(src_dir)

tcpobjs = \
	$(build_dir)/packet.o \
	$(build_dir)/arp.o \
	$(build_dir)/eth.o \
	$(build_dir)/ip.o \
	$(build_dir)/utils.o \
	$(build_dir)/timer.o \
	$(build_dir)/ipasm.o \
	$(build_dir)/udp.o \
	$(build_dir)/dns.o \
	$(build_dir)/trace.o

objs = \
	$(build_dir)/main.o \
	$(build_dir)/screen.o \
	$(build_dir)/input.o \
	$(build_dir)/ui.o \
	$(build_dir)/panel.o \
	$(build_dir)/tnfs.o \
	$(build_dir)/cfg.o \
	$(build_dir)/dlg_server.o \
	$(build_dir)/dlg_drive.o \
	$(build_dir)/local_client.o \
	$(build_dir)/tnfs_client.o \
	$(build_dir)/netw_mtcp.o \
	$(build_dir)/log.o
	

all : $(build_dir)/$(application).exe

clean :
	rm -f $(build_dir)/*.o
	rm -f $(build_dir)/*.exe
	rm -f $(build_dir)/*.map
	rm -f *.err *.map
	
$(build_dir):
	mkdir -p $(build_dir)

$(build_dir)/%.o : $(tcp_c_dir)/%.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<

$(build_dir)/ipasm.o : $(tcp_c_dir)/ipasm.asm | $(build_dir)
	wasm -fr=$(error_file) -0 $(memory_model) -fo=$@ $<

$(build_dir)/main.o : $(src_dir)/main.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<

$(build_dir)/screen.o : $(src_dir)/screen.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<
	
$(build_dir)/input.o : $(src_dir)/input.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<

$(build_dir)/ui.o : $(src_dir)/ui.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<
	
$(build_dir)/panel.o : $(src_dir)/panel.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<

$(build_dir)/tnfs.o : $(src_dir)/tnfs.c | $(build_dir)
	wcc $(compile_options) -fr=$(error_file) -fo=$@ $<	

$(build_dir)/cfg.o : $(src_dir)/cfg.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<

$(build_dir)/dlg_server.o : $(src_dir)/dlg_server.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<

$(build_dir)/dlg_drive.o : $(src_dir)/dlg_drive.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<

$(build_dir)/local_client.o : $(src_dir)/local_client.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<

$(build_dir)/tnfs_client.o : $(src_dir)/tnfs_client.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<	

$(build_dir)/netw_mtcp.o : $(src_dir)/netw_mtcp.cpp | $(build_dir)
	wpp $(compile_options) -fr=$(error_file) -fo=$@ $<

$(build_dir)/log.o : $(src_dir)/log.c | $(build_dir)
	wcc $(compile_options) -fr=$(error_file) -fo=$@ $<

$(build_dir)/$(application).exe : $(tcpobjs) $(objs)
	wlink system dos option map=$(build_dir)/$(application).map option eliminate option stack=4096 \
	name $@ \
	file { $^ }
