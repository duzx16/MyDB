use orderDB;
-- select with null condition
select * from orders where id is not null;
-- select with date
select * from orders where date > '2018-01-01';
-- select with different conditions
select * from orders where customer_id>310375;
select * from orders where customer_id=312195;
-- select with index
create index orders (customer_id);
select * from orders where customer_id=312195;
create index orders (food_id);
select * from orders where food_id>0;
-- select with string like
select * from restaurant where name like 'wu k_ng';
select * from restaurant where name like 'vivande%';

-- select with complex condition
select * from customer where gender='F' or id>314500 and id>314000;
select orders.id, food.name, quantity, price from orders, food where orders.food_id=food.id and quantity * price > 800.0;

-- select with aggregation
select restaurant_id, avg(price) from food;
select restaurant_id, avg(price) from food group by restaurant_id;
select restaurant_id, avg(price) from food where price>=60 group by restaurant_id;
select avg(price) from food where price>=60;
select food_id, avg(quantity) from orders group by food_id;

-- select with multiple tables
select *
from orders, customer, food
where food.id=orders.food_id and customer.id=orders.customer_id;

select *
from orders, food, restaurant
where restaurant.id=food.restaurant_id and food.id=orders.food_id and orders.quantity>1;
