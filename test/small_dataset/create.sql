create database school;
use school;

create table student(
sid int,
name varchar(20) not null,
sex varchar(20),
age int,
address varchar(20),
primary key(sid),
check(sex in ('man', 'woman')));

create table course (
cid int,
subject varchar(20) not null,
primary key (cid)
);

create table grade (
gid int,
sid int,
cid int,
score int not null,
primary key (gid)
);