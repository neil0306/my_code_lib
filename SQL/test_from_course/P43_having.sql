USE sql_invoicing;

-- case：我希望筛选出 total_sale > 500 的客户，怎么整？
SELECT 
	client_id,
    SUM(invoice_total) AS total_sales
FROM invoices
-- WHERE total_sales > 500  -- 这里不能用 WHERE，因为 total_sales 还没弄出来，而且 total_sales 需要基于GROUP BY才能得到结果（到where这里group by还没被执行），这里用 WHERE 是拿不到结果的
GROUP BY client_id
HAVING total_sales > 500;    -- HAVING 用来进一步筛选 GROUP BY 返回的结果


-- 注：
	-- where 用来【直接筛选】原来的表格或者有执行JOIN之后的表格的数据
    -- HAVING 用来【筛选GROUP BY】之后返回的【分组后的数据】
    # [另外] 
		# 只要表格中存在的列，where 条件都可以直接使用这些列名称来写过滤条件
        # having 只能使用 select 中出现过的列名称， select 中没有的一律不允许使用
    
-- case: 如果我们需要对每一个客户计算总销售额和发票数量，怎么做？
SELECT 
	client_id,
    SUM(invoice_total) AS total_sales,
    COUNT(*) AS number_of_invoices
FROM invoices
GROUP BY client_id
-- HAVING total_sales > 500 
HAVING total_sales > 500 AND number_of_invoices > 5  -- 执行多个语句， 用AND
;

-- exercise: 找出住在 VA， 消费超过$100的 customer
USE sql_store;

SELECT 
	c.customer_id,
	c.first_name,
    c.last_name,
    SUM(oi.quantity * oi.unit_price) AS spent
FROM orders o
JOIN order_items oi
	USING (order_id)
JOIN customers c 
	USING (customer_id)
WHERE c.state = 'VA'
GROUP BY c.customer_id
HAVING spent > 100;


-- 下面是答案：
-- 拆解：1. 找出谁住在 VA； 2. 找出他们的消费信息（orders); 3. 计算总消费，并筛选大于100的
SELECT 
	c.customer_id,
    c.first_name,
    c.last_name,
    SUM(oi.quantity * oi.unit_price) AS total_sales
FROM customers c
JOIN orders o 
	USING (customer_id)
JOIN order_items oi
	USING (order_id)
WHERE state = 'VA'
GROUP BY 						-- 注：通常来说，SELECT 中选中的列大部分都会用到 GROUP BY 中
	c.customer_id,
    c.first_name,
    c.last_name
HAVING total_sales > 100;


    
