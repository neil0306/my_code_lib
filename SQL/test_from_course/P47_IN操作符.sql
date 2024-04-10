-- Find the products that have never been ordered

USE sql_store;


SELECT *
FROM products
WHERE product_id NOT IN (
	SELECT 
		DISTINCT product_id  -- 找出所有被下过单的product id，并去重 (注意这个 sub-query 会返回多个结果）
	FROM order_items
);



-- exercise: Find clients without invoices
USE sql_invoicing;

SELECT *
FROM clients
WHERE client_id NOT IN(
	SELECT DISTINCT client_id
	FROM invoices);



