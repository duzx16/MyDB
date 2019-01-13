CREATE DATABASE orderDB;

USE orderDB;

CREATE TABLE restaurant (
  id INT(10) NOT NULL,
  name CHAR(25) NOT NULL,
  address CHAR(100),
  phone CHAR(20),
  rate FLOAT,
  PRIMARY KEY (id)
);

CREATE TABLE customer(
	id INT(10) NOT NULL,
	name CHAR(25) NOT NULL,
	gender CHAR(1) NOT NULL,
	PRIMARY KEY (id),
	check (gender in ('F', 'M'))
);

CREATE TABLE food(
	id INT(10) NOT NULL,
	restaurant_id INT(10),
	name CHAR(100) NOT NULL,
	price FLOAT NOT NULL,
	PRIMARY KEY (id),
	FOREIGN KEY (restaurant_id) REFERENCES restaurant(id)
);

CREATE TABLE orders(
	id INT(10) NOT NULL,
	customer_id INT(10) NOT NULL,
	food_id INT(10) NOT NULL,
	date DATE,
	quantity INT(10),
	PRIMARY KEY (id),
	FOREIGN KEY (customer_id) REFERENCES customer(id),
	FOREIGN KEY (food_id) REFERENCES food(id)
);
