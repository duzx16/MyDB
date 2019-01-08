use orderDB;
show tables;
update orders set id=-id;
select * from orders where id<0;
update orders set food_id=food_id+1000;
update customer set gender = 'U';
select * from customer;