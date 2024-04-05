SELECT *
FROM orders o
JOIN customers c
	ON o.customer_id = c.customer_id;
  
SELECT *
FROM orders o, customers c             -- 这里写多个表格
WHERE o.customer_id = c.customer_id;   -- JOIN xxx ON xx 的语句替换成 WHERE 语句，效果相同。

-- 虽然MySQL支持这种隐式JOIN条件，但是非常不建议使用，
	-- 因为如果一不小心漏了 WHERE 的条件，就会出发 CROSS JOIN, 
    -- 也就是表格中任意两条数据进行拼接，这会产生大量垃圾结果（如：正确处理的时候得到10条数据，CROSS JOIN之后得到100条这种情况），耗费系统性能