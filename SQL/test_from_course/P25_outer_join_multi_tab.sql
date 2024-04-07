SELECT 
	c.customer_id,
    c.first_name,
    o.order_id,
    sh.name AS shipper
FROM customers c
LEFT JOIN orders o
	ON c.customer_id = o.customer_id
LEFT JOIN shippers sh		 -- 再用一个outer join 把进行shipped的订单显示出来
	ON o.shipper_id = sh.shipper_id
ORDER BY c.customer_id;

-- tips：实际使用场景里，尽可能使用LEFT JOIN，而不使用RIGHT JOIN，因为会很难维护


-- exercise
SELECT 
	o.order_date,
	o.order_id,
    c.first_name AS customer,
    sh.name AS shipper,
    os.name AS status
FROM customers c
LEFT JOIN orders o
	ON c.customer_id = o.customer_id   -- 查看订单列表里的客户信息
LEFT JOIN shippers sh
	ON sh.shipper_id = o.shipper_id    -- 查看订单里的订单shipper
LEFT JOIN order_statuses os
	ON o.status = os.order_status_id   -- 进一步查看订单的 status
WHERE o.order_date IS NOT null
ORDER BY sh.name;


-- 答案
SELECT 
	o.order_id,
    o.order_date,
    c.first_name AS customer,
    sh.name AS shipper,
    os.name AS status
FROM  orders o
JOIN customers c
	ON o.customer_id = c.customer_id
LEFT JOIN shippers sh
	ON sh.shipper_id = o.shipper_id
JOIN order_statuses os
	ON os.order_status_id = o.status;


