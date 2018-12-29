use orderDB;
select avg(price), restaurant_id from food group by restaurant_id;
select * from food;
select * from food, customer where food.id = 200213;