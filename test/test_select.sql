use orderDB;
select * from orders where date is null;
select * from orders where date > '2018-01-01';
select * from orders where customer_id>310375;
select * from orders where customer_id=312195;
create index orders (customer_id);
select * from orders where customer_id=312195;
