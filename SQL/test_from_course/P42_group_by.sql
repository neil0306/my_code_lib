SELECT 
	SUM(invoice_total) AS total_sales		-- 统计了整个表
FROM invoices;				

-- case：如果想统计每一个客户的销售额怎么办？
SELECT 
	client_id,
	SUM(invoice_total) AS total_sales
FROM invoices
WHERE invoice_date >= "2019-07-01"     -- 只针对下半年的数据进行统计
GROUP BY client_id			-- 先按照client_id 分组，然后对每一个组执行 SUM(invoice_total), 得到每一个客户的销售额; 默认按照分组出现的顺序来排列
ORDER BY total_sales DESC;
	
    -- 注意书写顺序： SELECT -> FROM -> WHERE -> 【GROUP BY】 -> ORDER BY


-- case: 对多列数据进行 GROUP BY？
SELECT 
	state,
    city,
	SUM(invoice_total) AS total_sales
FROM invoices
JOIN clients 
	USING (client_id)
GROUP BY state, city;	    -- 查询不同 州&城市 的销售额; 同时对 state 和 city 进行分组 


-- exercise
SELECT 
	p.date,
    pm.name AS payment_method,
    SUM(p.amount) AS total_payments
FROM payments p
JOIN payment_methods pm
	ON p.payment_method = pm.payment_method_id
GROUP BY date, payment_method   -- 如果这两列数据存在排列组合，那么有数据的组合（合法的组合）将会被筛选出来
ORDER BY date;
