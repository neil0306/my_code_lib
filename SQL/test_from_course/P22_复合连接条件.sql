USE sql_store;

SELECT *
FROM order_items oi
JOIN order_item_notes oin
	ON oi.order_id = oin.order_id
    AND oi.product_id = oin.product_id;   -- 复合条件，用条件运算符 AND/OR 等连接
    
-- 当一个表格的primary key 不仅有一列时，我们在JOIN的时候就需要使用复合连接条件，用来确保JOIN的时候能唯一确定某一行数据
