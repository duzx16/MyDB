use orderDB;

-- insert illegal date
insert into orders values (1,309702,201329,'2014-09-31',5);
insert into orders values (2,309702,201329,'2010-02-29',5);
insert into orders values (3,309702,201329,'1900-02-29',5);

-- insert optional attribute
insert into restaurant values (3, 'test3', 'address','12345678', null);
insert into restaurant(name, id) values ('test1', 0001);
insert into restaurant(name) values ('test2');
select * from restaurant where id=1;
select * from restaurant where name='test1';
select * from restaurant where name='test2';
select * from restaurant where id=3;
delete from restaurant where rate is null;

-- check in
-- check value ... in
insert into customer values (300001,'CHAD CABELLO','U');
-- check foreign key
insert into food values (1,1,'Boiled dumplings',64.0236);
-- check primary key
insert into food values (200001,1613,'Boiled dumplings',64.0236);