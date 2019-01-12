use orderDB;
update orders set id=-id;
select * from orders where id<0;
update orders set id=-id;
select * from orders where id>0;

update customer set id=-id where gender='F';
select * from customer;
update customer set id=-id where id<0;
select * from customer;
-- check value ... in
update customer set gender = 'U';
-- check foreign key
update food set restaurant_id=1 where restaurant_id=1501;
-- check primary key
update customer set id=300002 where id=300001;
