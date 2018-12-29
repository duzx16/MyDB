use orderDB;
select avg(price), restaurant_id from food group by restaurant_id;
update food set price = price * 2 where id > 400000;
select * from food;