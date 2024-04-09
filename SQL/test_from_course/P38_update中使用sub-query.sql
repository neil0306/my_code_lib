USE sql_invoicing;

UPDATE invoices
SET 
	payment_total = invoice_total * 0.5,
    payment_date = due_date
WHERE client_id IN 						-- 如果sub-query 会返回多行数据，则这里的 where 需要用 IN
				(SELECT  client_id		-- sub-query, 用括号括起来，表示先执行这个子语句
				FROM clients
				WHERE state IN ('CA','NY'));


UPDATE invoices
SET 
	payment_total = invoice_total * 0.5,
    payment_date = due_date
WHERE payment_date is NULL;


-- exercise 
USE sql_store;

UPDATE orders 
SET	comments = "Golden customer"
WHERE order_id IN 
		(SELECT customer_id
		FROM customers
		WHERE points > 3000)




