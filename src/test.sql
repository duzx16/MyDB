drop database test;
create database test;
use database test;
create table student(id int, score float);
show table student;
create table student2(id int, score float, name char(10), check (id in (6, 7, 8)), check (name in ('Bob', 'John')), PRIMARY KEY (id, score), FOREIGN KEY (id) REFERENCES student(id));
show tables;