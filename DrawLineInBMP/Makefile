#  Makefile 
#  Auteur : Farès BELHADJ
#  Email  : amsi@up8.edu
#  Date   : 02/03/2020

# définition des commandes utilisées
CXX = g++
CC= gcc
ECHO = echo
RM = rm -f
TAR = tar
MKDIR = mkdir
CHMOD = chmod
CP = rsync -R

# déclaration des options du compilateur
CFLAGS = -Wall -O3
CXXFLAGS = -Wall -O3
CPPFLAGS = -I.
LDFLAGS = -lm

# définition des fichiers et dossiers
PROGNAME = DrawLineInBMP
VERSION = 1.1
distdir = $(PROGNAME)-$(VERSION)
HEADERS = images_io.h
SOURCES = DrawLineInBMP.cpp images_io.c
SUBOBJ = $(SOURCES:.cpp=.o)
OBJ = $(SUBOBJ:.c=.o)
DISTFILES = $(SOURCES) Makefile $(HEADERS) DrawLineInBMP.sln DrawLineInBMP.vcxproj

# Traitements automatiques des dépendances
all: $(PROGNAME)
$(PROGNAME): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(PROGNAME)
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
dist: distdir
	$(CHMOD) -R a+r $(distdir)
	$(TAR) zcvf $(distdir).tgz $(distdir)
	$(RM) -r $(distdir)
distdir: $(DISTFILES)
	$(RM) -r $(distdir)
	$(MKDIR) $(distdir)
	$(CHMOD) 777 $(distdir)
	$(CP) $(DISTFILES) $(distdir)
clean:
	@$(RM) -r $(PROGNAME) $(OBJ) *~ $(distdir).tgz gmon.out core.*
