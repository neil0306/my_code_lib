-- 例子1：将来自同一个表格的query结果整合到一个表格进行显示
SELECT 
	o.order_id,
    o.order_date,
    'Active' AS status
FROM orders o
WHERE order_date >= "2019-01-01"

UNION   -- 用UNION，将两个查询结果整合到一个表格中（注意这两个query结果里的【列的数量是相同的】）

SELECT 
	o.order_id,
    o.order_date,
    'Archive' AS status
FROM orders o
WHERE order_date < "2019-01-01";



-- 例子2：将来自不同表格的query结果整合到一个表格进行显示
SELECT first_name
FROM customers c
UNION    -- 整合结果
SELECT name
FROM shippers;



-- UNION 负责将表格的【行】数据进行链接
-- 注意要整合的两个结果必须满足【列的数量相同】
-- 最后得到的表格中，列的名称以【第一个query结果所指定的名字为准】，如第一个query的列名叫 AA，第二个query的列名叫 BB，最后UNION出来的列会叫作AA



-- exercise
	SELECT 
		c.customer_id,
		c.first_name,
		c.points,
		"Bronze" AS type
	FROM customers c
	WHERE c.points < 2000
UNION 		-- 第一次连接
	SELECT 
		c.customer_id,
		c.first_name,
		c.points,
		"Silver" AS type
	FROM customers c
	WHERE points BETWEEN 2000 AND 3000
UNION  -- 再次连接
	SELECT 
		c.customer_id,
		c.first_name,
		c.points,
		"Gold" AS type
	FROM customers c
	WHERE c.points > 3000
ORDER BY first_name  -- 对所有连接结果进行排序


