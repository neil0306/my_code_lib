-- Select invoices larger than all invoices of client 3

USE sql_invoicing;

SELECT 
	*
FROM invoices 
WHERE invoice_total > (		-- 原始版，使用sub-query， 由于使用了MAX，这里的 > 只会与单个数值进行比较
	SELECT 
		MAX(invoice_total)
	FROM invoices
	WHERE client_id = 3
);


SELECT 
	*
FROM invoices
WHERE invoice_total > ALL (   -- 由于 > 运算符只能比较单个数值，所以这里必须加上ALL才能将 invoices 里的所有 invoice_total 数据与 sub-query 的结果进行比较
	SELECT invoice_total
    FROM invoices
    WHERE client_id = 3
);

