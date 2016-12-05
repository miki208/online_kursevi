DIR = online_kursevi

.PHONY: all create trigger insert dist clean backup

all: create insert

create:
	mysql -u root -p < ddl.sql
	
insert: trigger
	mysql -u root -p < insert.sql
	
trigger:
	mysql -u root -p < trigger.sql
	
clean:
	-rm -f *.mwb.bak
	
dist: clean
	-tar -cz -C .. -f ../$(DIR).tar.gz $(DIR)
	
backup: dist
	-scp ../$(DIR).tar.gz mi13304@alas.matf.bg.ac.rs:backup/