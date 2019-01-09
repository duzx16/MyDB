use orderDB;
select * from restaurant where id=1;
insert into restaurant(name, id) values ('test', 1);
select * from restaurant where id=1;
delete from restaurant where rate is null;
select * from restaurant where id=1;