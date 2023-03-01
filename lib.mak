MF=Mf-$(KERNEL)-$(ARCH)-$(BUILD).mk

LIBTARGET=$(OBJDIR)/$(PROG)

V:=@

$(SNAME): $(MF) $(CSRC) $(CXXSRC)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(LIBDIR)
	@$(MAKE) -f $(MF) $(SNAME)

$(DNAME): $(MF) $(CSRC) $(CXXSRC)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(LIBDIR)
	@$(MAKE) -f $(MF) $(DNAME)

$(MF): Makefile $(MFDEPS)
#	for f in $(CSRC) ; do \
#		echo "Hey: a$(OBJDIR) $(f)" \
#		OF=$(OBJDIR)/`basename "$${f%.*}.o"` ; \
#	done
	@rm -f $(MF)
	@echo "CXX=$(CXX)" >> $(MF)
	@echo "CFLAGS=$(CFLAGS)" >> $(MF)
	@echo "CXXFLAGS=$(CXXFLAGS)" >> $(MF)
	@echo "LDFLAGS=$(LDFLAGS)" >> $(MF)
	@echo -n "OBJS=" >> $(MF)
	@for f in $(CSRC) ; do \
		OF=$(OBJDIR)/`basename "$${f%.*}.o"` ; \
		echo -n " $${OF}" >> $(MF) ;\
	done
	@for f in $(CXXSRC) ; do \
		OF=$(OBJDIR)/`basename "$${f%.*}.o"` ; \
		echo -n " $${OF}" >> $(MF) ;\
	done
	@echo "" >> $(MF)
	@echo "$(SNAME) : \$$(OBJS)" >> $(MF)
	@echo "	@echo Creating static library: $(SNAME)" >> $(MF)
	@echo "	$(V)\$$(AR) rcs $(SNAME) \$$(OBJS)" >> $(MF)
	@echo "" >> $(MF)
	@echo "$(DNAME) : \$$(OBJS)" >> $(MF)
	@echo "	@echo Creating dynamic library: $(DNAME)" >> $(MF)
	@echo "	$(V)\$$(LD) \$$(LDFLAGS) -shared -o $(DNAME) \$$(OBJS)" >> $(MF)
	@echo "" >> $(MF)
#	@echo "	\$$(CC) -o $(PROGTARGET) \$$(LDFLAGS) \$$(OBJS) $(LIBS)" >> $(MF)
	@for f in $(CSRC) ; do \
		OF=$(OBJDIR)/`basename "$${f%.*}.o"` ; \
		echo $${OF}: $$f  >> $(MF) ; \
		echo "	@if [ \"\$$(V)\" = \"@\" ]; then echo \"Compiling: $${f}\" ; fi" >> $(MF) ; \
		echo "	\$$(V)\$$(CC) -c \$$(CFLAGS) -o $${OF} $${f}" >> $(MF) ; \
	done
	@for f in $(CXXSRC) ; do \
		OF=$(OBJDIR)/`basename "$${f%.*}.o"` ; \
		echo $${OF}: $$f  >> $(MF) ; \
		echo "	@if [ \"\$$(V)\" = \"@\" ]; then echo \"Compiling: $${f}\" ; fi" >> $(MF) ; \
		echo "	\$$(V)\$$(CXX) -c \$$(CXXFLAGS) -o $${OF} $${f}" >> $(MF) ; \
	done


clean::
	rm -rf $(OBJDIR)
	rm -f $(SNAME)
	rm -f $(DNAME)
	rm -f $(MF)

#install: $(PROGTARGET)
#	mkdir -p $(INSTALL_BINDIR)
#	install $(PROGTARGET) $(INSTALL_BINDIR)/$(PROG)
