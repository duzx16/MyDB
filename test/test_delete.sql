use orderDB;
select * from restaurant where id=4;
insert into restaurant(name, id) values ('test4', 4);
insert into restaurant(name, id) values ('test5', 5);
select * from restaurant where id=4;
delete from restaurant where rate is null;
select * from restaurant where id=4;
delete from restaurant where id=5;
select * from restaurant where id=5;