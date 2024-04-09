USE sql_invoicing;

SELECT 
	MAX(invoice_total) AS highest,
    MIN(invoice_total) AS lowest,
    AVG(invoice_total) AS average,
    SUM(invoice_total * 1.1) AS total,
    -- COUNT(*) AS number_of_invoices
    COUNT(DISTINCT client_id) AS total_records  -- 去重后再计数
FROM invoices
WHERE invoice_date > '2019-07-01';

-- 以下函数会忽略 NULL 值
	-- MAX() -- 计算最大值
	-- MIN() -- 最小值
	-- AVG() -- 平均值
	-- SUM() -- 求和
	-- COUNT() -- 统计数量
		-- COUNT(*) 会把 NULL值 也一起统计

-- DISTINCT -- 不是一个函数，是要给【关键词】，效果是【去重】

-- 执行顺序：
	-- 1. FROM 先确定表格
    -- 2. WHERE 过滤数据
    -- 3. SELECT
		-- 1. 聚合函数括号内的表达式
		-- 2. 执行聚合函数


-- exercise
SELECT 
	"First half of 2019" AS 'date_range',
	 SUM(invoice_total) AS 'total_sales',
     SUM(payment_total) AS 'total_payments',
     SUM(invoice_total - payment_total) AS 'what_we_expect'
FROM invoices
WHERE invoice_date BETWEEN '2019-01-01' AND '2019-06-30'
UNION
SELECT 
	"Second half of 2019" AS 'date_range',
	 SUM(invoice_total) AS 'total_sales',
     SUM(payment_total) AS 'total_payments',
     SUM(invoice_total - payment_total) AS 'what_we_expect'
FROM invoices
WHERE invoice_date BETWEEN '2019-07-01' AND '2019-12-31'
UNION
SELECT 
	"Total" AS 'date_range',
	 SUM(invoice_total) AS 'total_sales',
     SUM(payment_total) AS 'total_payments',
     SUM(invoice_total - payment_total) AS 'what_we_expect'
FROM invoices