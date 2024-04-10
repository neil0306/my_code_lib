-- select clients that have an invoice

SELECT *
FROM clients
WHERE client_id IN(   -- 除了sub-query，也可以用JOIN 的方式
	SELECT DISTINCT client_id
    FROM invoices
);

# 使用exist
SELECT *
FROM clients c
WHERE EXISTS (    -- 使用 EXISTS 关键词
	SELECT client_id
    FROM invoices
    WHERE client_id = c.client_id   # 注意，这里用了 correlated sub-quary 的方式
									# EXISTS 将依靠这个 WHERE 语句来判断是否为 true （是否exist）
);


-- 分析：
	# 在前面的IN语句中，WHERE语句里的 sub-query 虽然只会执行一次，但是执行的结果会【暂存到内存里】
    # 此时，如果执行返回的临时结果很多的话，将非常耗内存，耗资源
    # 换成 EXISTS 的case，这里虽然使用了 correlated sub-query 的形式，会导致sub-query 执行很多遍，但是每次执行都不会占用太多内存来暂存结果
    # 这是一个 trade-off，得看具体是内存重要还是CPU执行时间重要。 （如果内存消耗大，还是用 correlated 的方式更优）


-- exercise： Find the products that have never been ordered
USE sql_store;

SELECT *
FROM products p
WHERE NOT EXISTS (
	SELECT product_id
    FROM order_items
    WHERE product_id = p.product_id
)

