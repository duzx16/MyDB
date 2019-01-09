use orderDB;
show tables;
update orders set id=-id;
select * from orders where id<0;
update orders set id=-id;
select * from orders where id>0;
update customer set gender = 'U';
