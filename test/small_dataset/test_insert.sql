use school;
insert into student values (1001, 'Li', 'man', 20, 'sdwf');
insert into student values (1002, 'Xu', 'man', 23, 'sdgm');
insert into student values (1003, 'zhao', 'woman', 25, 'sdwf');
insert into student values (1004, 'Si', 'woman', 24, 'default');

insert into course values (1, 'oracle');
insert into course values (2, 'English');
insert into course values (3, 'Java');
insert into course values (4, 'SQL');

insert into grade values (101,1001,1,60);
insert into grade values (102,1001,1,70);
insert into grade values (103,1001,1,40);
insert into grade values (104,1001,2,80);
insert into grade values (105,1001,2,90);
insert into grade values (106,1001,3,60);
insert into grade values (107,1002,1,65);
insert into grade(gid, sid, cid) values (108,1003,4);