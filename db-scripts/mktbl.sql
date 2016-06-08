CREATE TABLE patient  (
id integer,
pesel numeric(11),
name varchar(100),
last_name varchar(100),
tel numeric(11),
sex varchar(1),
race varchar(20),
birth_date date,
birth_city varchar(100),
country varchar(100),
PRIMARY KEY(id)
);


CREATE TABLE examination (
id integer,
patient_id integer,
name varchar,
result_path text,
date date,
FOREIGN KEY(patient_id) REFERENCES patient(id),
PRIMARY KEY(id)
);
