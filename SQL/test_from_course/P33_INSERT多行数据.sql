INSERT INTO shippers (name)
VALUES ('shipper1'),
	('shipper2'),
    ('shipper3');

-- 插入多行数据： 同样是指定插入数据的列名称，然后插入的多行数值在 VALUE 部分用多个括号指定， 即 "()," 


-- exercise
INSERT INTO products(name, 
					quantity_in_stock,
                    unit_price)

VALUE ('name 1', 1, 1.1),
	('name 2', 2, 2.2),
    ('name 3', 3, 3.3)
