use orderDB;

select * from orders, food, restaurant where restaurant.id=food.restaurant_id and food.id=orders.food_id and orders.quantity>1;