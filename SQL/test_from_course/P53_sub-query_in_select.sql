USE sql_invoicing;
SELECT 
	invoice_id,
    invoice_total,
	(SELECT AVG(invoice_total)    -- SELECT 中的 sub-query
        FROM invoices
    ) AS invoice_average,
    
    invoice_total - (SELECT invoice_average) AS difference    -- 由于前面一个sub-query在执行逻辑中已经完成，这里可以直接SELECT，不然就只能再写一次上面的sub-query了
FROM invoices;

-- 每一个 关键词（如SELECT, FROM, WHERE)里的 sub-query 都是按【从上往下】的顺序执行的，
   -- 执行结果一般会存放到内存里，故排在下面的sub-query可以复用临时结果。  


-- exercise：
SELECT 
	client_id,
    name,
    (SELECT 
		SUM(invoice_total) 
	FROM invoices
	WHERE c.client_id = client_id ) AS total_sales,  -- 筛选出有发票的client， 这里会把没有发票的客户填充上 NULL 值
    (SELECT 
		AVG(invoice_total) 
	FROM invoices) AS average,
    (SELECT total_sales) - (SELECT average) AS difference    -- 注意这里不能直接用前面的临时结果，而需要用sub-query
FROM clients c





