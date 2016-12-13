DIR = online_kursevi
PROGRAM = program.out
SRC = program.c
FLAGS = -g -Wall `mysql_config --cflags --libs`

.PHONY: all create trigger insert dist clean backup publish

all: create insert $(PROGRAM)

create:
	mysql -u root -p < ddl.sql
	
insert: trigger
	mysql -u root -p < insert.sql
	
trigger:
	mysql -u root -p < trigger.sql
	
$(PROGRAM): $(SRC)
	gcc $(SRC) -o $(PROGRAM) $(FLAGS)

clean:
	-rm -f *.mwb.bak *.out
	
dist: clean
	-tar -cz -C .. -f ../$(DIR).tar.gz $(DIR)
	
publish:
	-tar -cz -f ../$(DIR)_pub.tar.gz ddl.sql trigger.sql insert.sql Makefile opis.doc model.mwb $(SRC)
	
backup: dist
	-scp ../$(DIR).tar.gz mi13304@alas.matf.bg.ac.rs:backup/
