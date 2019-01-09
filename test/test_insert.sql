use orderDB;
insert into restaurant(name, id) values ('test', 0001);
insert into restaurant(name) values ('test2');
select * from restaurant where id=1;
select * from restaurant where name='test';