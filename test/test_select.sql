use orderDB;
select avg(price), restaurant_id from food group by restaurant_id;
update food set id = id * 2 where price > 80.0;
select * from food;