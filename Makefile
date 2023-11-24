EXE_SERVER = bin/server/main.out
SRC_SERVER = $(wildcard src/server/*.c)
HEADERS_SERVER = $(wildcard src/server/*.h)
BDIR_SERVER = build/server/
OBJ_SERVER = $(HEADERS_SERVER:src/server/%.h=$(BDIR_SERVER)%.o)
DEPS_SERVER = $(HEADERS_SERVER:src/server/%.h=$(BDIR_SERVER)%.d)

EXE_CLIENT = bin/client/main.out
SRC_CLIENT = $(wildcard src/client/*.cpp)
HEADERS_CLIENT = $(wildcard src/client/*.h)
BDIR_CLIENT = build/client/
OBJ_CLIENT = $(HEADERS_CLIENT:src/client/%.h=$(BDIR_CLIENT)%.o)
DEPS_CLIENT = $(HEADERS_CLIENT:src/client/%.h=$(BDIR_CLIENT)%.d)

FLAG_DEBUG = -DMAP -DTRACE -g -Wall# -Wextra -Werror# -ansi -Og# -fsanitize=address -fno-omit-frame-pointer
FLAG_FINAL = -pedantic -Wall -Wextra -Werror -DNDEBUG -O3
LIBS = 
CXX = gcc

MAKEFILE_LIST := Makefile
TARGET := $(word 1, $(MAKECMDGOALS))
VALID_TARGETS = $(shell awk -F':' '/^[a-zA-Z0-9][^$#\/\t=]*:([^=]|$$)/ {split($$1,A,/ /);print A[1]}' $(MAKEFILE_LIST))

# --------------------  -------------------- #

.DEFAULT:
	$(info Cible $(TARGET) inconnue)
	@make -s help

.PHONY:help # pour ne pas confondre help et le fichier help (même si il n'y en a pas)
help : ## affiche cette aide
		@egrep -h '\s##\s' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "} \
		{printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'

# --------------------  -------------------- #

.PHONY:server
server : $(EXE_SERVER) ## (defaut) compile le serveur debug

$(EXE_SERVER) : $(SRC_SERVER) # linkage $(EXE)
	$(CXX) $(FLAG_DEBUG) -o $@ $^ $(LIBS)

# $(BDIR_SERVER)%.d : src/server/%.c # dependances
# 	$(CXX) $(FLAG_DEBUG) -MMD -c $< -o $@ $(LIBS)

# $(BDIR_SERVER)%.o : src/server/%.c $(DEPS_SERVER) # compilation
# 	$(CXX) $(FLAG_DEBUG) -c $< -o $@ $(LIBS)

# --------------------  -------------------- #

.PHONY:client
client : $(EXE_CLIENT) ## (defaut) compile le client debug

$(EXE_CLIENT) : $(OBJ_CLIENT) # linkage $(EXE)
	$(CXX) $(FLAG_DEBUG) -o $@ $^ $(LIBS)

$(BDIR_CLIENT)%.d : src/client/%.c # dependances
	$(CXX) $(FLAG_DEBUG) -MMD -c $< -o $@ $(LIBS)

$(BDIR_CLIENT)%.o : src/client/%.c $(DEPS_CLIENT) # compilation
	$(CXX) $(FLAG_DEBUG) -c $< -o $@ $(LIBS)

# --------------------  -------------------- #

.PHONY:final
final : chflag clean $(EXE_SERVER) $(EXE_CLIENT) ## compile le serveur et le client en optimisé sans trace ni map

.PHONY:chflag
chflag :
	FLAG_DEBUG = FLAG_FINAL

# --------------------  -------------------- #

.PHONY:prep
prep : ## create all needed directories
	for i in bin build deps
	do
		for j in server client
		do
			mkdir -p $i/$j
		done
	done

# --------------------  -------------------- #

.PHONY:clean
clean : ## supprime les dépendances, les fichiers objets, la doc et les executables
	@echo "suppression des dependances"
	@rm -f $(DEPS_SERVER) $(DEPS_CLIENT)
	@echo "suppression des objets"
	@rm -f $(OBJ_SERVER) $(OBJ_CLIENT)
	@echo "suppression de la documentation"
	@rm -rf doc/html* doc/xml*
	@echo "suppression des executables"
	@rm -f $(EXE_SERVER) $(EXE_CLIENT)

# --------------------  -------------------- #

.PHONY:verif
verif :
	echo "fichiers sources : " $(SRC); \
	echo "fichiers objets : " $(OBJ); \
	echo "fichiers executables : " $(EXE); \
	echo "flag : " $(FLAG); \
	echo "dependances : " $(DEPS); \
	echo "cibles : " $(VALID_TARGETS); \
	echo "sources : " $(SRC_FINAL); \

# --------------------  -------------------- #

.PHONY:doc
doc : ## génère la documentation avec doxygen
	@echo "suppression de la documentation"
	@rm -rf doc/html* doc/xml*
	@echo "génération de la nouvelle documentation"
	@doxygen doc/image.doxy > /dev/null
	@echo "ouverture de la doc"
	@firefox doc/html/index.html &

# --------------------  -------------------- #

ifneq ("$(wildcard tools/newClasse.sh)","")
.PHONY:new
new :
	@read -p "entrez le nom de la nouvelle classe : " n; \
	./tools/newClasse.sh $$n
endif

# --------------------  -------------------- #

.PHONY:zip
zip : veryclean ## prépare pour le rendu et met le dépot dans un zip
	zip -r ../B3104.zip . -i \*.cpp \*.h Makefile README.md doc/rapport.pdf doc/image.doxy logsTests\* tests\* -x tools/SClasse.cpp tools/SClasse.h






