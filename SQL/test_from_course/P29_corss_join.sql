-- 显式写法
SELECT 
	c.first_name AS customer,
    p.name AS product
FROM customers c
CROSS JOIN products p   -- cross join 本质上是将两个表格的每一行进行【两两拼接】
ORDER BY c.first_name;


-- 隐式写法
SELECT 
	c.first_name AS customer,
    p.name AS product
FROM customers c, products p  -- 直接在 FROM 语句中写两个表格，就会触发 CROSS JOIN！！！ (一定要小心！）
ORDER BY c.first_name;


-- 通常CROSS JOIN的使用场景是： 我们想知道两个表格中数据的所有排列组合

-- exercise
SELECT *
FROM shippers sh, products		-- 隐式写法
ORDER BY sh.name; 

SELECT *
FROM shippers sh
CROSS JOIN products			-- 显式写法
ORDER BY sh.name; 


