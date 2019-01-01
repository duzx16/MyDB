use orderDB;
select * from restaurant, food where restaurant.id = food.restaurant_id;
select avg(price), restaurant_id from food group by restaurant_id;