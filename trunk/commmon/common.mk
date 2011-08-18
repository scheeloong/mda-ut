# for QT 4
ifndef NF2_ROOT
	QTI= -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -DQT4=1
	QTL= -L/usr/lib/qt4 -lQtGui
	MOC=/usr/share/qt4/bin/moc
else
#for QT 3
	QTI= -I/usr/lib/qt-3.3/include 
	QTL= -L/usr/lib/qt-3.3/lib -lqui -lqt-mt
	MOC=/usr/bin/moc
endif


ifdef MDA_SUB
	QTI= -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -DQT4=1
	QTL= -L/usr/lib/qt4 -lQtGui
	MOC=/usr/bin/moc
endif


ifdef SYMPHONYEDA
	QTI= -I/usr/qt/3/include
	QTL= -L/usr/qt/3/lib -lqui -lqt-mt
	MOC=/usr/qt/3/bin/moc
endif

#############################################################

LZFDIR=../common/lzf
LIBLZF=$(LZFDIR)/liblzf.a

$(LIBLZF): 
	make -C $(LZFDIR)

#############################################################

PROTOCOLDIR=../common/protocol
LIBPROTOCOL=$(PROTOCOLDIR)/libprotocol.a

$(LIBPROTOCOL): 
	make -C $(PROTOCOLDIR)

#############################################################
IMGDIR=../common/img

#############################################################

HOUGHDIR=../common/hough
LIBHOUGH=$(HOUGHDIR)/libhough.a

$(LIBHOUGH): 
	make -C $(HOUGHDIR)

#############################################################

FIFODIR=../common/iffifo
LIBFIFO=$(FIFODIR)/libfifo.a

$(LIBFIFO): 
	make -C $(FIFODIR)

#############################################################

HOUGHINC=../common/hough
SHAPEINC=../common/hough/shapes

# determines if you use qt4 or qt3.3
ifndef NF2_ROOT
QT4=1  # this is QT4 setting
else
QT4=0  # this is ECF setting
endif

