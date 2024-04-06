USE sql_store;

SELECT 
	c.customer_id,
    c.first_name,
    o.order_id
FROM customers c
JOIN orders o
	ON c.customer_id = o.customer_id   -- 当指定了JOIN的条件时，系统只会返回满足这个条件的JOIN
ORDER BY c.customer_id;


-- 使用outer JOIN： left join 和 right join
SELECT 
	c.customer_id,
    c.first_name,
    o.order_id
FROM customers c
LEFT JOIN orders o   -- left join： 不管是否满足下面的 ON 条件，位于【左侧】的表格 customers 里的内容都会被系统返回，然后由select进行选取
	ON c.customer_id = o.customer_id   -- 当指定了JOIN的条件时，系统只会返回满足这个条件的JOIN
ORDER BY c.customer_id;


SELECT 
	c.customer_id,
    c.first_name,
    o.order_id
FROM customers c
RIGHT JOIN orders o   -- right join： 不管是否满足下面的 ON 条件，位于【右侧】的表格 customers 里的内容都会被系统返回，然后由select进行选取
	ON c.customer_id = o.customer_id   -- 当指定了JOIN的条件时，系统只会返回满足这个条件的JOIN
ORDER BY c.customer_id;

-- 对于 JOIN 来说，INNER 和 OUTER 这两个关键词都是可以省略的

-- exercise
