USE sql_invoicing;

SELECT *
FROM clients
WHERE client_id NOT IN(
	SELECT DISTINCT client_id
	FROM invoices);
    
-- 把sub-query 改写成 join

SELECT *
FROM clients
LEFT JOIN invoices
	USING(client_id)
WHERE invoice_id  IS NULL;


-- sub-query 与 JOIN 之间的选择原则：
	# 性能更好
    # 可读性更高


-- exercise
USE sql_store;

SELECT 	
	customer_id,
    first_name,
    last_name
FROM customers c
WHERE c.customer_id IN (  # sub-query 版本
	SELECT 
		customer_id
	FROM order_items
    JOIN orders 
		USING (order_id)
    WHERE product_id = 3
);


SELECT 	
	DISTINCT customer_id,
    first_name,
    last_name
FROM customers c
JOIN orders o          		-- JOIN 版本 （更易读）
	USING (customer_id)
JOIN order_items oi
	USING(order_id)
WHERE oi.product_id = 3;
	
