CC = gcc
LDFLAGS = -lpthread
TARGET = main.c
OBJFILE = main
BANKERS_PATH = bankers-algorithm
BESTFIT_PATH = best-fit-algorithm
all: bankers bestfit
bankers: $(BANKERS_PATH)/$(TARGET)
	$(CC) $(BANKERS_PATH)/$(TARGET) -o $(BANKERS_PATH)/$(OBJFILE) $(LDFLAGS)
	@echo '[INFO] Use make runBankers args="<arguments>" to run the bankers algorithm'
runBankers: $(BANKERS_PATH)/$(OBJFILE)
	cd $(BANKERS_PATH) && ./$(OBJFILE) $(args)
bestfit: $(BESTFIT_PATH)/$(TARGET)
	$(CC) $(BESTFIT_PATH)/$(TARGET) -o $(BESTFIT_PATH)/$(OBJFILE) $(LDFLAGS)
	@echo '[INFO] Use make runBestfit args="<arguments>" to run the bestfit algorithm'
runBestfit: $(BESTFIT_PATH)/$(OBJFILE)
	cd $(BESTFIT_PATH) && ./$(OBJFILE) $(args)
clean:
	rm -f $(BANKERS_PATH)/$(OBJFILE) $(BESTFIT_PATH)/$(OBJFILE)