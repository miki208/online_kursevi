.PHONY: all create trigger insert

all: create insert

create:
	mysql -u root -p < ddl.sql
	
insert: trigger
	mysql -u root -p < insert.sql
	
trigger:
	mysql -u root -p < trigger.sql