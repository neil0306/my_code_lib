-- insert into 用来插入一行数据

INSERT INTO customers(							-- 指定插入数据的表格
						first_name,				-- 指定要插入的数据对应的列名称
						last_name,
                        birth_date,
                        address,
                        city,
                        state)   
VALUES (
	-- DEFAULT, 		-- 对于那些勾选了【AI，Auto-increase】属性的列，推荐使用DEFAULT，让系统自动赋值，防止冲突
	'John',
    'Smith',
    '1990-01-01',		-- DATE 在 customer 表格里没有勾选【NN，not null】属性，意味着这个数据是【可选】的
    -- NULL,				-- phone number 也是可选的
    'address',
    'city',
    'CA'  
    -- DEFAULT
    );    
	
-- 使用VALUE指定一行数据时，下方VALUE里提供的值 要与 前面INSERT INTO中指定的列顺序匹配（但是不要求 INSERT INTO里写的顺序与表格中的顺序完全对应）。
