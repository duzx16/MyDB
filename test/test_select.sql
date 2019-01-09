use orderDB;
select * from orders where id is not null;
select * from orders where date > '2018-01-01';
select * from orders where customer_id>310375;
select * from orders where customer_id=312195;
create index orders (customer_id);
select * from orders where customer_id=312195;
create index orders (food_id);
select * from orders where food_id>0;

select * from restaurant where name like 'wu k_ng';
select * from restaurant where name like 'vivande%';

select *
from orders, customer, food
where food.id=orders.food_id and customer.id=orders.customer_id;

select restaurant_id, avg(price) from food;
select restaurant_id, avg(price) from food group by restaurant_id;
select restaurant_id, avg(price) from food where price>=60 group by restaurant_id;
select avg(price) from food where price>=60;
select food_id, avg(quantity) from orders group by food_id;
