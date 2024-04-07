USE sql_store;

SELECT 
	o.order_id,
    c.first_name,
    sh.name AS shipper
FROM orders o
JOIN customers c
	-- ON o.customer_id = c.customer_id
    USING (customer_id) -- 效果等同于上面的ON语句
LEFT JOIN shippers sh
    USING(shipper_id);
    
-- 注意： 使用USING 语句必须保证两个表格中有【相同的列名称】

SELECT *
FROM order_items oi
JOIN order_item_notes oin
-- 	ON oi.order_id = oin.order_id AND			-- 回顾：表格中的primary key有多个时，需要用AND写多个条件
-- 		oi.product_id = oin.product_id
	USING(order_id, product_id);				-- 用USING语句，就可以简化这个语句


-- exercise
USE sql_invoicing;

SELECT 
	p.date,
    c.name AS client,
    p.amount,
    pm.name

FROM payments p
LEFT JOIN payment_methods pm
	ON pm.payment_method_id = p.payment_method   -- 列的名字不一样，无法用USING
JOIN clients c
	USING(client_id)    					     -- 列的名称一样，可以用USING


