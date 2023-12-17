 CC = gcc
 COPTS = -g -Wall -fPIC

 LOPTS = -lgmp

 DEBUG += -D  _DEBUG_
 DEBUG += -D  _DEBUG2_

all:
	$(CC)    $(COPTS) $(DEBUG) -D_DRIVER_BASE58_ inbase58.c $(LOPTS)

clean:
	rm -f *.o a.out

