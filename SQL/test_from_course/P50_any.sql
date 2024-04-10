-- select clients with at lease two invoices

SELECT *
FROm clients
WHERE client_id IN (
	SELECT client_id		-- 选出所有至少有两张发票的 client_id
	FROM invoices
	GROUP BY client_id
	HAVING COUNT(*) >= 2  -- 统计每一个client_id 持有的发票数量，并过滤出 >=2 张的
);


SELECT *
FROm clients
WHERE client_id = ANY (     -- 筛选client里的数据，看看是否能匹配上 sub-query 中的【任意一个结果】
	SELECT client_id		-- 选出所有至少有两张发票的 client_id
	FROM invoices
	GROUP BY client_id
	HAVING COUNT(*) >= 2  -- 统计每一个client_id 持有的发票数量，并过滤出 >=2 张的
);


-- 注：
	#  【=IN】 操作符 等价于 【ANY】关键词

