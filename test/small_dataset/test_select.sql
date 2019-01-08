use school;
select student.sid, student.name, grade.gid, course.subject, grade.score
from student, course, grade
where student.sid=grade.sid and course.cid=grade.cid;
select cid, avg(score) from grade;
select cid, avg(score) from grade group by cid;
select cid, avg(score) from grade where score>=60 group by cid;
select sid, avg(score) from grade group by sid;
select avg(score) from grade where score>=60;