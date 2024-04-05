USE sql_inventory;   -- USE 用来指定默认的数据库，有点类似C++的命名空间

SELECT *
FROM sql_store.order_items oi    -- 此时，如果我们要用的表格位于另一个数据库里，那么就需要用【数据库名.表格名】来调用
JOIN products p
	ON oi.product_id = p.product_id;