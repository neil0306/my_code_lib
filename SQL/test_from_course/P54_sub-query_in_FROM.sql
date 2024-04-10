
SELECT *
FROM (
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
) AS sales_summary    -- FROM 中使用 sub-query 时，起别名是【必须的】，不管用不用得上
WHERE total_sales IS NOT NULL

-- 在FROM语句中写 sub-query 是允许的，而且需要注意：
	# FROM 里的 sub-query 一定要起别名
    # FROM 里的 sub-query 通常不会太复杂