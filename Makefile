EXE = bin/main.out
SRC = $(wildcard src/*.cpp)
HEADERS = $(wildcard src/*.h)
OBJ = $(HEADERS:src/%.h=build/%.o)
DEPS = $(HEADERS:src/%.h=build/%.d)

FLAG_DEBUG = -DMAP -DTRACE -g -Wall -Wextra -Werror# -std=c++11# -ansi -Og# -fsanitize=address -fno-omit-frame-pointer
FLAG_FINAL = -pedantic -Wall -Wextra -Werror -std=c++11 -DNDEBUG -O3
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

.PHONY:main
main : $(EXE) ## (defaut) compile l'executable final en optimisé sans trace ni map

$(EXE) : $(OBJ) # linkage $(EXE)
	$(CXX) $(FLAG) -o $@ $^ $(LIBS)

$(BDIR)%.d : src/%.cpp # dependances
	$(CXX) $(FLAG) -MMD -c $< -o $@ $(LIBS)

$(BDIR)%.o : src/%.cpp $(DEPS) # compilation
	$(CXX) $(FLAG) -c $< -o $@ $(LIBS)

# --------------------  -------------------- #

.PHONY:clean
clean : ## supprime les dépendances, les fichiers objets, la doc et les executables
	@echo "suppression des dependances"
	@rm -f $(DEPS)
	@echo "suppression des objets"
	@rm -f $(OBJ)
	@echo "suppression des données temporaires applicatives (/data/tmp/)"
	@rm data/tmp/*
	@echo "suppression de la documentation"
	@rm -rf doc/html* doc/xml*
	@echo "suppression des executables"
	@rm -f $(EXE)

# --------------------  -------------------- #

.PHONY:verif
verif : ## affiche le contenu des variables du makefile
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
new : ## génère une nouvelle classe /!\ n'est valable que si les squelettes et le script newClasse.sh sont présents dans tools/
	@read -p "entrez le nom de la nouvelle classe : " n; \
	./tools/newClasse.sh $$n
endif

# --------------------  -------------------- #

.PHONY:update
update : ## fais les modifications git (git commit puis git push et enfin git status)
	$(info Modifs git :)
	@git diff --stat
	@git add *
	@read -p "Entrez le commentaire de la modification : " commentaire; \
	git commit -m "$$commentaire" -a
	@git push
	@git status

# --------------------  -------------------- #

.PHONY:zip
zip : veryclean ## prépare pour le rendu et met le dépot dans un zip
	zip -r ../B3104.zip . -i \*.cpp \*.h Makefile README.md doc/rapport.pdf doc/image.doxy logsTests\* tests\* -x tools/SClasse.cpp tools/SClasse.h






